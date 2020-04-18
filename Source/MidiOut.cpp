#include"MidiOut.h"

#include"PluginEditor.h"
#include"HexFunc.h"
#include<string>
#include<regex>
#include<vector>
#include<assert.h>

using std::string;
using std::regex;
using std::smatch;
using std::regex_search;
using std::vector;


MidiOut::MidiOut(RolandControlAudioProcessor &p) : midiDevNum_(-1), open_(false), device_(NULL), processor_(p) {
	init();
}

MidiOut::~MidiOut(){
	midiOutReset(device_);
	midiOutClose(device_);
	device_ = NULL;
}

void MidiOut::init(){

	MIDIOUTCAPS caps;

	// Find device
	for(int i = 0; i < (int)midiOutGetNumDevs(); i++){
		midiOutGetDevCaps(i, &caps, sizeof(MIDIOUTCAPS));

		if((std::string)caps.szPname == "Roland Digital Piano"){
			midiDevNum_ = i;
			break;
		}
	}

	// Midi out not found
	if(midiDevNum_ == -1)
		return;

	MMRESULT rs = midiOutOpen(&device_, midiDevNum_, NULL, 0, CALLBACK_NULL);

	if(rs != MMSYSERR_NOERROR)
		return;

	open_ = true;
}

void MidiOut::reset(){

}

bool MidiOut::open(){
	return open_;
}

void MidiOut::shortMsg(DWORD data){

	if(!open_)
		return;

	midiOutShortMsg(device_, data);
	//setStatusDW(data);
}

void MidiOut::cc(int channel, int cc, int val){
	
	union{ DWORD dwData; BYTE bData[4]; } msg;

	msg.bData[0] = 0xb0 + channel;
	msg.bData[1] = cc;
	msg.bData[2] = val;
	msg.bData[3] = 0;

	shortMsg(msg.dwData);
}

void MidiOut::setTone(int channel, int pc, int msb, int lsb){

	if(!open_)
		return;

	// Bank select
	cc(channel, 0, msb);
	cc(channel, 0x20, lsb);

	// Program change
	union{ DWORD dwData; BYTE bData[4]; } msg;
	msg.bData[2] = 0;
	msg.bData[3] = 0;

	msg.bData[0] = 0xc0 + channel;
	msg.bData[1] = pc;
	shortMsg(msg.dwData);
}

void MidiOut::sysexData(const string& address, char value){
	char values[] = {value};
	sysexData(address, values, 1);
}


void MidiOut::sysexData(const string& address, const char* values, int size){

	if(!open_)
		return;


	// Get address
	regex rgx("([\\da-f]{2})\\s*([\\da-f]{2})\\s*([\\da-f]{2})");
	smatch matches;

	// Output string from matches.str appears to have issues
	string a = address;

	// Get address bytes
	char addr1 = 0, addr2 = 0, addr3 = 0;

	if(regex_search(a, matches, rgx)){
		addr1 = hexStringToByte(matches.str(1));
		addr2 = hexStringToByte(matches.str(2));
		addr3 = hexStringToByte(matches.str(3));
	}

	// MIDI data
	vector<char> data;

	// 'Header' bytes
	data.push_back(0xf0);
	data.push_back(0x41);
	data.push_back(0x10);
	data.push_back(0x42);
	data.push_back(0x12);

	// Address bytes
	data.push_back(addr1);
	data.push_back(addr2);
	data.push_back(addr3);

	int checksum = addr1 + addr2 + addr3;

	for(int i = 0; i < size; i++){
		data.push_back(values[i]);
		checksum += values[i];
	}

	// Checksum
	data.push_back((checksum & 0x7f) ^ 0x7f);

	// EOX
	data.push_back(0xf7);


	MIDIHDR hdr;
	hdr.lpData = data.data();
	hdr.dwBufferLength = data.size();
	hdr.dwBytesRecorded = data.size();
	hdr.dwFlags = 0;

	// Send message
	midiOutPrepareHeader(device_, &hdr, sizeof(hdr));
	auto error = midiOutLongMsg(device_, &hdr, sizeof(hdr));

	// Check MIDI error
	//assert(error == 0);

	// Wait to unprepare header
	if(error == 0)
		while((hdr.dwFlags & MHDR_DONE) != MHDR_DONE);

	midiOutUnprepareHeader(device_, &hdr, sizeof(hdr));


	//setStatusBytes(data, 11);

	/*
	string status = "MIDI out: ";

	for(int i = 0; i < data.size(); i++)
		status += byteToHex((BYTE&)data.at(i)) + " ";

	status += "\n";

	OutputDebugString(status.c_str());
	*/
}

void MidiOut::EFXPartSysex(int channel, Effect effect){

	// Values
	int depth	= (int)*processor_.getParameters().getRawParameterValue("efxDepthch" + std::to_string(channel));
	int param1	= (int)*processor_.getParameters().getRawParameterValue("efxParam1ch" + std::to_string(channel));
	int param2	= (int)*processor_.getParameters().getRawParameterValue("efxParam2ch" + std::to_string(channel));


	// Get default parameter values if per-part parameter is disabled
	bool hasParam1 = effect.pparam1 != -1;
	bool hasParam2 = effect.pparam2 != -1;
	int param1Default = 0;
	int param2Default = 0;

	char address[32];
	unsigned char c;

	if(hasParam1){
		c = effect.pparam1 + 3;
		sprintf(address, "sysex 40 03 %s", byteToHex(c).c_str());
		param1Default = (int)*processor_.getParameters().getRawParameterValue(address);
	}

	if(hasParam2){
		c = effect.pparam2 + 3;
		sprintf(address, "sysex 40 03 %s", byteToHex(c).c_str());
		param2Default	= (int)*processor_.getParameters().getRawParameterValue(address);
	}

	char data[] = {
		// EFX Type (disable if depth = 0)
		depth == 0 ? 0 : effect.msb,
		depth == 0 ? 0 : effect.lsb,

		// EFX macro
		0x00,

		// EFX depth and parameters
		depth,
		hasParam1 ? (param1 != -1 ? param1 : param1Default) : 0,
		hasParam2 ? (param2 != -1 ? param2 : param2Default) : 0,
	};

	// Correct channel values for SysEx data
	if(channel == 9)		channel = 0;
	else if(channel < 9)	channel++;

	// Set address for each channel
	c = channel;
	sprintf(address, "40 4%c 23", byteToHex(c)[1]);
	sysexData(address, data, sizeof(data));
}

// Disabled due to crashing
/*
void MidiOut::setStatusDW(DWORD& dw){

	// Disabled due to crashing
	return;

	union{ DWORD dwData; char bData[4]; } data;
	data.dwData = dw;

	setStatusBytes(data.bData, 4);
}

void MidiOut::setStatusBytes(char bytes[], int size){

	// Disabled due to crashing
	return;

	auto editor = (RolandControlAudioProcessorEditor*)processor_.getActiveEditor();

	if(editor == nullptr)
		return;

	string status = "MIDI out: ";

	for(int i = 0; i < size; i++)
		status += byteToHex((BYTE&)bytes[i]) + " ";

	//editor->setStatus(status);
}
*/

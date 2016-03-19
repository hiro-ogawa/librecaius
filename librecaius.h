// =================
// Recaius Library
// Version 0.1
// 2015/3/13 Hiro Ogawa
// =================
#ifndef LIBRECAIUS_H
#define LIBRECAIUS_H

#include <string>
#include <vector>
using namespace std;

class CRecaiusRecognition
{
public:
  enum ResultType{
    ONE_BEST,
    NBEST,
    CONFNET,
  };

  CRecaiusRecognition();
  ~CRecaiusRecognition();

  int LoadConfigFile(string fname);
  int SetID(string _id);
  int SetPassword(string _password);
  int SetModelID(int _model_id);

  int Login();
  int Logout();
  string SendVoice(int voice_id, vector<unsigned char> data);
  string SendWaveFile(string fname);
  string GetResult();

private:
  string id;
  string password;
  string uuid;

  string orig_url;
  string base_url;

  long curl_verbose;

  int model_id;
  int result_type;
};

class CSynth
{
public:
  typedef struct st_speaker{
    string speaker_id;
    string lang;
    string style;
    string description;
    string emotion;
    string tts_type;
    string dic_type;
  }speaker;

  vector<speaker> speakers;

  CSynth();
  ~CSynth();

  int LoadConfigFile(string fname);
  int SetID(string _id);
  int SetPassword(string _password);
  int SetSpeaker(int speaker_id);
  int SetLang(string _lang);
  int SetSpeakerID(string _id);

  vector<unsigned char> GetSpeechWaveFromPlainText(string text);
  int GetSpeakerList();

private:
  string id;
  string password;
  string lang;
  string speaker_id;

  string orig_url;
  string base_url;

  long curl_verbose;
};

#endif

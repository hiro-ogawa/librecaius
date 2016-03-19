#include <fstream>
#include <iostream>
using namespace std;

#include <json/json.h>
#include <json/writer.h>
#include <json/reader.h>

#include "librecaius.h"

namespace{
  Json::Value root;
}

void ParseResult(string chunk)
{
  if(chunk == "")
    return;

  Json::Value result;
  Json::Reader reader;
  reader.parse(chunk, result);
  // Json::StyledWriter writer;
  // cout << writer.write(result);
  // return;

  for(int i=0;i<result.size();i++)
  {
    string type = result[i].get("type","").asString();
    cout << result[i].get("type","").asString() << endl;
    if(type == "RESULT")
    {
      for(int j=0;j<result[i]["result"].size();j++)
        cout << result[i]["result"][j]["str"].asString() << endl;
    }
  }
}

void RecognitionTest(string lang)
{
  int ret;
  CRecaiusRecognition recog;

  string id, pass;
  int model_id;
  id = root["recog"][lang.c_str()].get("id","").asString();
  pass = root["recog"][lang.c_str()].get("password","").asString();
  model_id = root["recog"][lang.c_str()].get("model_id","0").asInt();

  if(model_id == 0)
  {
    cout << "RecognitionTest: LoadError" << endl;
    return;
  }

  recog.SetID(id);
  recog.SetPassword(pass);
  recog.SetModelID(model_id);

  string fname = "recog_test_" + lang + ".wav";
  cout << fname << endl;

  ret = recog.Login();

  string chunk;
  chunk += recog.SendWaveFile(fname);
  ParseResult(chunk);

  for(int i=0;i<10;i++)
  {
    sleep(2);
    // usleep(100e3);
    chunk = recog.GetResult();
    ParseResult(chunk);
  }
  ret = recog.Logout();
}


void SynthesisTest()
{
  CSynth synth;

  string id, pass;
  id = root["synth"].get("id","").asString();
  pass = root["synth"].get("password","").asString();

  synth.SetID(id);
  synth.SetPassword(pass);
  synth.GetSpeakerList();

  for(int i=0;i<synth.speakers.size();i++)
  {
    string fname = "synth-test-" + synth.speakers[i].lang + ".txt";
    cout << fname << endl;

    synth.SetSpeaker(i);

    ifstream ifs(fname);
    if (ifs.fail())
    {
        cerr << "Load Text Failed" << endl;
        return;
    }
    istreambuf_iterator<char> it(ifs);
    istreambuf_iterator<char> last;
    string input_text(it, last);

    vector<unsigned char> raw;
    // raw = synth.GetSpeechWaveFromPlainText(input_text);
    raw = synth.GetSpeechWaveFromPlainText(input_text);

    string wavfname = "synth-test-" + synth.speakers[i].speaker_id + ".wav";
    cout << wavfname << endl;
    ofstream ofs(wavfname, ios::binary);
    ofs.write((const char*)&raw[0], raw.size());
  }
}

int main()
{
  ifstream ifs("recaius_id.json");
  Json::Reader reader;
  reader.parse(ifs, root);

  // RecognitionTest("ja_JP");
  // RecognitionTest("en_US");
  // RecognitionTest("zh_CN");
  SynthesisTest();

  return 0;
}

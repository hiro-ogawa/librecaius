// =================
// Recaius Library
// Version 0.1
// 2015/3/13 Hiro Ogawa
// =================

#include <string>
#include <iostream>
#include <vector>
using namespace std;

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
using namespace boost::property_tree;

#include <curl/curl.h>

#include <jsoncpp/json/json.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/reader.h>

#include "librecaius.h"

namespace
{
  string cmd_login = "login";
  string cmd_logout = "logout";
  string cmd_voice = "voice";
  string cmd_result = "result";

  string cmd_text2wave = "plaintext2speechwave";
  string cmd_getsplist = "get_speaker_list";

  enum HTTP_RES {
    HTTP_RES_OK = 200
  };

  size_t callbackWrite(char *ptr, size_t size, size_t nmemb, string *stream)
  {
    int dataLength = size * nmemb;
    stream->append(ptr, dataLength);
    return dataLength;
  }
  size_t callbackWriteRaw(char *ptr, size_t size, size_t nmemb, vector<unsigned char> *stream)
  {
    int dataLength = size * nmemb;
    // stream->resize(dataLength);
    // memcpy(&stream[0], ptr, dataLength);
    for(int i=0;i<dataLength;i++){
      stream->push_back(ptr[i]);
    }
    return dataLength;
  }
}

CRecaiusRecognition::CRecaiusRecognition()
{
  id = "";
  password = "";
  uuid = "";
  model_id = 0;

  curl_verbose = 0;

  orig_url = "https://try-api.recaius.jp/asr/v1/";
  base_url = orig_url;
}

CRecaiusRecognition::~CRecaiusRecognition()
{
  Logout();
}

int CRecaiusRecognition::LoadConfigFile(string fname)
{
  return 0;
}

int CRecaiusRecognition::SetID(string _id)
{
  id = _id;
  return 0;
}

int CRecaiusRecognition::SetPassword(string _password)
{
  password = _password;
  return 0;
}

int CRecaiusRecognition::SetModelID(int _model_id)
{
  model_id = _model_id;
  return 0;
}

int CRecaiusRecognition::Login()
{
  //とりあえずログアウト
  Logout();

  //ログインデータ準備
  string header = "Content-Type:application/json";
  string url = base_url + cmd_login;

  // JSON形式のデータ用意
  Json::Value root;
  root["id"] = id;
  root["password"] = password;
  Json::Value model;
  model["model_id"] = model_id;
  model["resulttype"] = "nbest";
  root["model"] = model;
  Json::FastWriter writer;
  string json_string = writer.write(root);

  cout << url << endl;
  cout << header << endl;
  cout << json_string;

  //ログイン処理
  CURLcode ret;
  CURL *hnd;
  struct curl_slist *slist1;

  string chunk;

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, header.c_str());

  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, json_string.c_str());
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(hnd, CURLOPT_VERBOSE, curl_verbose);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callbackWrite);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &chunk);

  ret = curl_easy_perform(hnd);

  if(ret == CURLE_OK)
  {
    long res_code;
    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &res_code);
    cout << "Responce Code: " << res_code << endl;
    string content_type;

    switch(res_code)
    {
    case HTTP_RES_OK:
      char* ct;
      curl_easy_getinfo(hnd, CURLINFO_CONTENT_TYPE, &ct);
      if(ct == NULL)
        content_type = "NULL";
      else
        content_type = ct;

      cout << "Content Type: "<< content_type << endl;

      uuid = chunk;
      base_url = orig_url + uuid + "/";

      break;
    }
  }

  curl_easy_cleanup(hnd);
  hnd = NULL;
  curl_slist_free_all(slist1);
  slist1 = NULL;

  return ret;
}

int CRecaiusRecognition::Logout()
{
  if(uuid == ""){
    return 0;
  }

  //ログインデータ準備
  string url = base_url + cmd_logout;
  cout << url << endl;
  //
  // //Recaius APIアクセス
  CURLcode ret;
  CURL *hnd;

  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(hnd, CURLOPT_VERBOSE, curl_verbose);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

  ret = curl_easy_perform(hnd);

  if(ret == CURLE_OK)
  {
    long res_code;
    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &res_code);
    cout << "Responce Code: " << res_code << endl;
    string content_type;

    switch(res_code)
    {
    case HTTP_RES_OK:
      char* ct;
      curl_easy_getinfo(hnd, CURLINFO_CONTENT_TYPE, &ct);
      if(ct == NULL)
        content_type = "NULL";
      else
        content_type = ct;

      cout << "Content Type: "<< content_type << endl;

      break;
    }
  }

  curl_easy_cleanup(hnd);
  hnd = NULL;

  uuid = "";
  base_url = orig_url;

  return (int)ret;
}

string CRecaiusRecognition::SendVoice(int voice_id, vector<unsigned char> data)
{
  CURLcode ret;
  CURL *hnd;
  struct curl_httppost *post1;
  struct curl_httppost *postend;
  struct curl_slist *slist1;

  string chunk;

  cout << data.size() << endl;

  post1 = NULL;
  postend = NULL;
  stringstream stream;
  stream << voice_id;
  curl_formadd(&post1, &postend,
               CURLFORM_COPYNAME, "voiceid",
               CURLFORM_COPYCONTENTS, stream.str().c_str(),
               CURLFORM_END);
  if(data.size())
  {
    curl_formadd(&post1, &postend,
                 CURLFORM_COPYNAME, "voice",
                 CURLFORM_COPYCONTENTS, &data[0],
                 CURLFORM_CONTENTSLENGTH, data.size(),
                 CURLFORM_CONTENTTYPE, "application/octet-stream",
                 CURLFORM_END);
  }
  else
  {
    curl_formadd(&post1, &postend,
                 CURLFORM_COPYNAME, "voice",
                 CURLFORM_COPYCONTENTS, "",
                 CURLFORM_CONTENTTYPE, "application/octet-stream",
                 CURLFORM_END);
  }
  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "Content-Type:multipart/form-data");

  string url = base_url + cmd_voice;
  cout << url << endl;

  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
  curl_easy_setopt(hnd, CURLOPT_HTTPPOST, post1);
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "PUT");
  curl_easy_setopt(hnd, CURLOPT_VERBOSE, curl_verbose);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callbackWrite);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &chunk);

  ret = curl_easy_perform(hnd);

  if(ret == CURLE_OK)
  {
    long res_code;
    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &res_code);
    cout << "Responce Code: " << res_code << endl;
    string content_type;

    switch(res_code)
    {
    case HTTP_RES_OK:
      char* ct;
      curl_easy_getinfo(hnd, CURLINFO_CONTENT_TYPE, &ct);
      if(ct == NULL)
        content_type = "NULL";
      else
        content_type = ct;

      cout << "Content Type: "<< content_type << endl;

      break;
    }
  }

  curl_easy_cleanup(hnd);
  curl_formfree(post1);
  curl_slist_free_all(slist1);

  return chunk;
}

string CRecaiusRecognition::SendWaveFile(string fname)
{
  vector<unsigned char> wave_data;

  //バイナリファイル読み込み
  ifstream ifs((const char*)fname.c_str(), ios::in | ios::binary);
  ifs.seekg(0, fstream::end);
  unsigned int eofPos = ifs.tellg();
  ifs.clear();
  ifs.seekg(0, fstream::beg);
  unsigned int begPos = ifs.tellg();
  unsigned int size = eofPos - begPos;

  wave_data.resize(size);
  ifs.read((char*)&wave_data[0], wave_data.size());

  // cout << wave_data.size() << endl;
  // cout << wave_data.size()/44100 << endl;
  // cout << wave_data.size()%44100 << endl;

  string chunk;
  chunk += SendVoice(1, wave_data);
  vector<unsigned char> wave_null(0);
  chunk += SendVoice(2, wave_null);

  return chunk;
}

string CRecaiusRecognition::GetResult()
{
  CURLcode ret;
  CURL *hnd;

  string chunk;

  string url = base_url +  cmd_result;
  cout << url << endl;

  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
  curl_easy_setopt(hnd, CURLOPT_VERBOSE, curl_verbose);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callbackWrite);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &chunk);

  ret = curl_easy_perform(hnd);

  long res_code;
  if(ret == CURLE_OK)
  {
    curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &res_code);
    cout << "Responce Code: " << res_code << endl;
    string content_type;

    switch(res_code)
    {
    case HTTP_RES_OK:
      char* ct;
      curl_easy_getinfo(hnd, CURLINFO_CONTENT_TYPE, &ct);
      if(ct == NULL)
        content_type = "NULL";
      else
        content_type = ct;

      cout << "Content Type: "<< content_type << endl;

      break;
    }
  }

  curl_easy_cleanup(hnd);
  hnd = NULL;

  if(res_code == HTTP_RES_OK)
  {
    // cout << chunk << endl;

    // JSON形式のデータ用意
    Json::Value root;
    Json::Reader reader;
    bool success = reader.parse(chunk, root);

    return chunk;
  }

  return ("");
}


// 音声合成

namespace{
}

CSynth::CSynth()
{
  id = "";
  password = "";
  lang = "";
  speaker_id = "";

  curl_verbose = 0;

  speakers.clear();

  orig_url = "https://try-api.recaius.jp/tts/v1/";
  base_url = orig_url;
}

CSynth::~CSynth()
{

}

int CSynth::SetID(string _id)
{
  id = _id;
  return 0;
}

int CSynth::SetPassword(string _password)
{
  password = _password;
  return 0;
}

int CSynth::SetSpeaker(int speaker_id)
{
  SetLang(speakers[speaker_id].lang);
  SetSpeakerID(speakers[speaker_id].speaker_id);
  return 0;
}

int CSynth::SetLang(string _lang)
{
  lang = _lang;
  return 0;
}

int CSynth::SetSpeakerID(string _id)
{
  speaker_id = _id;
  return 0;
}

vector<unsigned char> CSynth::GetSpeechWaveFromPlainText(string text)
{
  // JSON形式のデータ用意
  Json::Value json_value;
  json_value["id"] = id;
  json_value["password"] = password;
  json_value["plain_text"] = text;
  json_value["lang"] = lang;
  json_value["speaker_id"] = speaker_id;
  Json::FastWriter writer;
  string json_string = writer.write(json_value);

  //recaiusにアクセス
  CURLcode ret;
  CURL *hnd;
  struct curl_slist *slist1;

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "Content-Type:application/json");

  string url = base_url + cmd_text2wave;

  cout << url << endl;
  cout << json_string << endl;

  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, json_string.c_str());
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(hnd, CURLOPT_VERBOSE, curl_verbose);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

  vector<unsigned char> raw;
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callbackWriteRaw);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &raw);

  ret = curl_easy_perform(hnd);

  long res_code;
  curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &res_code);
  cout << "Responce Code: " << res_code << endl;
  string content_type;
  char* ct;
  curl_easy_getinfo(hnd, CURLINFO_CONTENT_TYPE, &ct);
  if(ct == NULL)
    content_type = "NULL";
  else
    content_type = ct;
  cout << "Content Type: "<< content_type << endl;

  curl_easy_cleanup(hnd);
  hnd = NULL;
  curl_slist_free_all(slist1);
  slist1 = NULL;

  // cout << chunk << endl;

  return raw;
}


//  文字列を置換する
std::string Replace( std::string String1, std::string String2, std::string String3 )
{
    std::string::size_type  Pos( String1.find( String2 ) );

    while( Pos != std::string::npos )
    {
        String1.replace( Pos, String2.length(), String3 );
        Pos = String1.find( String2, Pos + String3.length() );
    }

    return String1;
}

int CSynth::GetSpeakerList()
{
  // JSON形式のデータ用意
  Json::Value json_value;
  json_value["id"] = id;
  json_value["password"] = password;
  Json::FastWriter writer;
  string json_string = writer.write(json_value);
  json_string.pop_back();
  cout << json_string << endl;

  //recaiusにアクセス
  CURLcode ret;
  CURL *hnd;
  struct curl_slist *slist1;

  slist1 = NULL;
  slist1 = curl_slist_append(slist1, "Content-Type:application/json");

  string url = base_url + cmd_getsplist;
  cout << url << endl;

  hnd = curl_easy_init();
  curl_easy_setopt(hnd, CURLOPT_URL, url.c_str());
  curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, json_string.c_str());
  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
  curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
  curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(hnd, CURLOPT_VERBOSE, curl_verbose);
  curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);

  string chunk;
  curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, callbackWrite);
  curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &chunk);

  ret = curl_easy_perform(hnd);

  curl_easy_cleanup(hnd);
  hnd = NULL;
  curl_slist_free_all(slist1);
  slist1 = NULL;

  // cout << chunk << endl;

  ptree pt;
  istringstream is1(chunk);
  read_xml(is1, pt);
  string body = pt.get<string>("response.body");

  istringstream is2(body);
  read_xml(is2, pt);

  //ひとりずつ処理
  speakers.clear();
  BOOST_FOREACH(ptree::value_type &v, pt.get_child("speaker_group"))
  {
    speaker sp;
    sp.speaker_id = v.second.get<string>("speaker_id");
    sp.lang = v.second.get<string>("lang");
    sp.style = v.second.get<string>("style");
    sp.description = v.second.get<string>("description");
    sp.emotion = v.second.get<string>("emotion");
    sp.tts_type = v.second.get<string>("tts_type");
    sp.dic_type = v.second.get<string>("dic_type");

    sp.lang = Replace(sp.lang, ",", "-");

    // cout << v.second.get<string>("alias") << endl;
    speakers.push_back(sp);
  }

  return (int)ret;
}

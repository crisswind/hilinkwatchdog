#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include <curl/curl.h>

#define _DEBUG_
#define _MAX_RETRY_CT_ 5
#define _LOOP_TIMEOUT_ 45

char cmd[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
int status[] = {0,0,0};
int fd1,rd;
char bufor[128];
std::string modem_;
std::string test_modem;

int tryb = 0; 	//1 -session


std::string session_id;
std::string token_id;
int connection_status = 0;
                         


//-----------------------------------------------------------------------------------------
std::string execute_(const char * str)
{
   if (!str)
     return "";
   std::string ret;
   FILE* fp;   
   char result [1000];
   memset(result, 0x00, 1000);
   fp = popen(str,"r");
   fread(result,1,sizeof(result),fp);
   fclose (fp);
#ifdef _DEBUG_   
   printf("exec:res:%s\n", result);
#endif
   if (result)
     ret = result;

   return ret; 
}

//-----------------------------------------------------------------------------------------
std::string execute2_(const char * cmd)
{
   if (!cmd)
     return "";
   std::string ret;
   FILE* fp;
   char result [1000];
   memset(result, 0x00, 1000);
   char result2 [1000];
   memset(result2, 0x00, 1000);
   fp = popen(cmd,"r");
   fread(result,1,sizeof(result),fp);
   fread(result2,1,sizeof(result2),fp);
   fclose (fp);
#ifdef _DEBUG_    
   printf("exec:res:%s\n", result);
#endif   
   if (result)
     ret = result;
   if (result2)
     ret += result2;

   return ret;
}  



//-----------------------------------------------------------------------------------------
int function_pt(void *ptr, size_t size, size_t nmemb, void *stream)
{
    std::string ret = (const char*)ptr;
  if (tryb == 1)
  {
    
    int pos0 = ret.find("<SesInfo>");
    int pos1 = ret.find("</SesInfo>");
    if (pos1 > pos0 && pos0 > 0 && pos1 < ret.length())
    {
     session_id = ret.substr(pos0+9, pos1-pos0 -9);    
    }
    pos0 = ret.find("<TokInfo>");
    pos1 = ret.find("</TokInfo>");
    if (pos1 > pos0 && pos0 > 0 && pos1 < ret.length())
    {
     token_id = ret.substr(pos0+9, pos1-pos0 -9);    
    }       
    
  }
  else
  if (tryb == 2)
  {
      if (!ret.empty() && ret.length() > 50)
      {
        int pos0 = ret.find("<ConnectionStatus>");
        int pos1 = ret.find("</ConnectionStatus>");
      
        std::string status = ret.substr(pos0+18, pos1-pos0-18);        
        if (!status.empty() && status.length() < 6)
          connection_status = atoi(status.c_str());    
     }  
  }
  else
  if (tryb==7)
    printf("%s\n", ptr);
  
}

//-----------------------------------------------------------------------------------------
int __call__(std::string url, int cookie = 0, int post = 0)
{
  CURL *curl;  
  CURLcode res;
  curl = curl_easy_init();
  if(curl) {
    if (cookie == 1)
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");
    if (post == 1)
      curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_COOKIE, session_id.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, function_pt);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
  }
  else
   printf("unable to init curl\n");
return 0;
}
 

void get_session()
{
  tryb = 1;
  __call__("http://192.168.8.1/api/webserver/SesTokInfo");
}

void get_status()
{
      tryb = 2;
      connection_status = 0;
      __call__("http://192.168.8.1/api/monitoring/status", 1);
      connection_status;
}

//-----------------------------------------------------------------------------------------
void get_smslist()
{
        std::string command = "curl -X POST -d \"<request><PageIndex>1</PageIndex><ReadCount>20</ReadCount><BoxType>1</BoxType><SortType>0</SortType><Ascending>0</Ascending><UnreadPreferred>0</UnreadPreferred></request>\" http://192.168.8.1/api/sms/sms-list -H \"__RequestVerificationToken: ";
        command += token_id;
        command += "\" --cookie ";
        command += session_id;
        command += " -H \"Content-Type: text/xml\"";
#ifdef _DEBUG_         
        printf("executing: %s\n", command.c_str());
#endif        
        execute2_(command.c_str());
}

//-----------------------------------------------------------------------------------------
void send_sms(std::string phone)
{
        std::string command = "curl -X POST -d \"<?xml version='1.0' encoding='UTF-8'?><request><Index>-1</Index><Phones><Phone>";
        command += phone;
        command += "</Phone></Phones><Sca></Sca><Content>HELLO</Content><Length>-1</Length><Reserved>1</Reserved><Date>-1</Date><SendType></SendType></request>\"";
        command += " http://192.168.8.1/api/sms/send-sms -H \"__RequestVerificationToken: ";
        command += token_id;
        command += "\" --cookie ";
        command += session_id;
        command += " -H \"Content-Type: text/xml\"";
#ifdef _DEBUG_ 
        printf("executing: %s\n", command.c_str());
#endif        
        execute_(command.c_str());
}

//-----------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
      int errct = 0;     
      
      do
      {
        connection_status = 0;
        session_id = "";
        token_id = "";
      
      get_session();
      
      if (errct > _MAX_RETRY_CT_)
       system("/sbin/reboot");
      
/*      
      get_smslist();
      send_sms("+48XXXXXXXXX");
      
      */
      
      
      get_status();

      if ( connection_status==901)
      {
#ifdef _DEBUG_       
        printf("status: OK\n");
#endif        
        errct = 0;
      }
      else
      if (connection_status > 0 && connection_status < 19999)
      {
        errct ++;
#ifdef _DEBUG_         
        printf("ERROR: status: %d\n", status);
        printf("restarting modem!\n");
#endif        
        std::string command = "curl -X POST -d \"<request><Control>1</Control></request>\" http://192.168.8.1/api/device/control -H \"__RequestVerificationToken: ";
        command += token_id;
        command += "\" --cookie ";
        command += session_id;
        command += " -H \"Content-Type: text/xml\"";
#ifdef _DEBUG_         
        printf("executing: %s\n", command.c_str());
#endif        
        system(command.c_str());
        sleep(20);
        
        execute_("/bin/hilink.sh");
        
        std::string verify = execute_("lsusb | grep 12d1:1f01");
        if (verify.length() > 10)
         {
#ifdef _DEBUG_          
          printf("retrying - modem hung\n");
#endif          
          execute_("/bin/hilink.sh");         
         }
        
        if (errct > _MAX_RETRY_CT_)
          execute_("/sbin/reboot");
               
      }
      else
       errct++;
      
      sleep(_LOOP_TIMEOUT_);

      }while(1);
      
   return 0;
}

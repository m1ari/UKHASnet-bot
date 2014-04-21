#include <jansson.h>
#include "config.h"
#include "irc/server.h"

// TODO Add mutexes around objects
// Method to save out json
// How to deal with config reload

namespace UKHASnet {

	Config::Config() {
		json=NULL;
		filename="";
	}

	Config::~Config() {
		json_decref(json);
	}

	void Config::setFile(std::string f){
		filename=f;
	}

	void Config::loadConfig(){
		FILE *fp=NULL;
		json_error_t err;

		fp = fopen(filename.c_str(), "r");
		if (fp != NULL){
			json=json_loadf(fp,0,&err);	// Newer version of jansoon lib
			fclose(fp);
		} else {
			fprintf(stderr, "Failed to open %s\n",filename.c_str());
		}
		if (json!=NULL){
			// Do Stuff
		} else {
 			if (fp!=NULL){
				printf("%d %d %d\n%s\n", err.line, err.column, err.position, err.text);
			}
		}
	}

	size_t Config::getNumIrcServers() const {
		json_t *obj=NULL;
		int val=0;
		if (json!=NULL){
			obj=json_object_get(json,"irc");
			if (json_is_array(obj)){
				val=json_array_size(obj);
			}
		}
		return val;
	}
	irc::Server Config::getIrcServer(int server) const {
		irc::Server s;
		json_t *obj=NULL;
		json_t *item=NULL;
		if (json!=NULL){
			obj=json_object_get(json,"irc");
			if (json_is_array(obj)){
				obj=json_array_get(obj,server);
				if (json_is_object(obj)){
					item=json_object_get(obj,"server");
					if(json_is_string(item)){
						s.setName(json_string_value(item));
						s.setServer(json_string_value(item));
					}

					item=json_object_get(obj,"nick");
					if(json_is_string(item)){
						s.setNick(json_string_value(item));
					}

					item=json_object_get(obj,"username");
					if(json_is_string(item)){
						s.setUser(json_string_value(item));
					}

					item=json_object_get(obj,"realname");
					if(json_is_string(item)){
						s.setRealname(json_string_value(item));
					}

					item=json_object_get(obj,"password");
					if(json_is_string(item)){
						s.setPass(json_string_value(item));
					}

					//channels
				}
			}
		}
		return s;
	}

	std::string Config::getString(int count, ... ) const {
		json_t *obj=NULL;
		std::string ret;
		va_list vl;
		char *str;
		va_start(vl, count);
		
		ret="";
		if ( json!= NULL){
			obj=json;
			for (int i=0; i< count; i++){
				str=va_arg(vl, char *);
				printf("Testing (%02d): %s\n",i,str);
				if (json_is_object(obj)){
					obj=json_object_get(obj,str);
				} else {
					printf("Error %s isn't an object\n",str);
				}
			}
			if (json_is_string(obj)){
				ret=json_string_value(obj);
			}
		}
		va_end(vl);

		return ret;
	}
	std::string Config::getArray(int count, ... ) const {
		//  "channels": ["#ukhasnet"]
	}

	void Config::saveConfig() const{
		// rename old config file
		// open filename to save
		// Dump json
		// close file

	}
}

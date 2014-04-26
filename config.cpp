#include <jansson.h>
#include <list>
#include <string>
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

	void Config::saveConfig() const{
		// rename old config file
		// open filename to save
		// Dump json
		// close file
	}

	std::list<std::string> Config::getIRCServerList() const {
		json_t *obj=NULL;
		json_t *val;
		const char *key;
		std::list<std::string> out;
		if (json!=NULL){
			obj=json_object_get(json,"irc");
			if (json_is_object(obj)){
				json_object_foreach(obj,key,val){
					out.push_back(key);
				}
			}
		}
		return out;
	}
	irc::Server Config::getIRCServer(std::string server) const {
		irc::Server s;
		json_t *obj=NULL;
		json_t *item=NULL;
		if (json!=NULL){
			obj=json_object_get(json,"irc");
			if (json_is_object(obj)){
				obj=json_object_get(obj,server.c_str());
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
				}
			}
		}
		return s;
	}

	std::list<std::string> Config::getIRCChannels(std::string server) const{
		std::list<std::string> out;
		json_t *obj;
		size_t index;
		json_t *val;

		if (json!=NULL){
			obj=json_object_get(json,"irc");
			if (json_is_object(obj)){
				obj=json_object_get(obj,server.c_str());
				if (json_is_object(obj)){
					obj=json_object_get(obj,"channels");
					if(json_is_array(obj)){
						json_array_foreach(obj,index,val){
							out.push_back(json_string_value(val));
						}
					}
				}
			}
		}
		return out;
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

}

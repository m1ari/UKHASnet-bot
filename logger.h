#ifndef logger_h_
#define logger_h_

/* Maintains the file handles for a log file and logrotate (occurs for the
 * first message logged passed midnight)
 */

namespace UKHASnet {

class Logger {
	private:
		FILE *logfd;
		time_t logtime;
		std::string path;
		std::string name;

		void openLog(struct tm *tm_now);
		void closeLog(struct tm *tm_now);

	public:
		Logger();
		Logger(std::string path, std::string name);
		~Logger();
		void setPath(std::string path);
		void setName(std::string name);
		void setFile(std::string file);
		void writeLog(std::string msg);
		void writeLog(std::string type, std::string msg);

	protected:
};

}
#endif

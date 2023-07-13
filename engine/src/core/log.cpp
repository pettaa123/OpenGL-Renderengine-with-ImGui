#include "log.h"
#include <stack>
#include <iostream>
#include <format>
#include "terminalColor.h"

namespace Log {
	static Level logLevel = Level::INFO;

	void setLogLevel(Level level) {
		logLevel = level;
	}

	Level getLogLevel() {
		return logLevel;
	}

	static std::string delimiter = "\n";

	void setDelimiter(std::string delimiter) {
		Log::delimiter = delimiter;
	}

	void debug(std::string format) {
		if (logLevel != Level::NONE) {
			std::string message = "[DEBUG]: " + format + delimiter;
			setColor(Color::DEBUG);
			std::cout << message;
			setColor(Color::NORMAL);
		}
	}

	void info(std::string format) {
		if (logLevel <= Level::INFO) {
			std::string message = "[INFO]: " + format + delimiter;
			setColor(Color::INFO);
			std::cout << message;
			setColor(Color::NORMAL);
		}
	}

	void warn(std::string format) {
		if (logLevel <= Level::WARNING) {
			std::string message = "[WARN]: " + format + delimiter;
			setColor(Color::WARNING);
			std::cout << message;
			setColor(Color::NORMAL);
		}
	}

	void error(std::string format) {
		if (logLevel <= Level::ERROR) {
			std::string message = "[ERROR]: " + format + delimiter;
			setColor(Color::ERROR);
			std::cout << message;
			setColor(Color::NORMAL);
		}
	}

	void fatal(std::string format) {
		if (logLevel <= Level::FATAL) {
			std::string message = "[FATAL]: " + format + delimiter;
			setColor(Color::FATAL);
			std::cout << message;
			setColor(Color::NORMAL);
		}
	}

	void print(std::string format) {
		setColor(Color::NORMAL);
		std::cout << format;
	}

	void print(TerminalColorPair color, std::string format) {
		setColor(color);
		std::cout << format;
	}
}

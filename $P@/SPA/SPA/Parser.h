#pragma once
class Parser
{
public:
	static vector<vector<string>> tokenized_codes;

	Parser(vector<string> codings);
	~Parser(void);
	bool IsEmpty(string str);
	
	bool is_number(const std::string& str);
	void AddTables(vector<string> list, string newtoken);
	vector<string> tokenizer(string line);
};

//TODO: For Nick, put your parser into this controller

#include "EmobiixField.c"

#include <stdio.h>

#include <stdio.h>

void txt2inc(const char *inputfilename, const char *outputfilename, const char *structname)
{
	int c;
	int loop = 0;
	FILE *input, *output;
	
	input = fopen(inputfilename, "r");
	output = fopen(outputfilename, "w");
	fprintf(output, "static const char *%s = \n", structname);

	do {
		c = fgetc(input);
		
		if (c == EOF) {
			if (loop != 0)
				fprintf(output, "\"");
			fprintf(output, ";\n");
			break;
		}

		if (loop == 0)
			fprintf(output, "\"");

		switch (c) {
			case '"':
				fprintf(output, "\\\"");
				loop += 2;
				break;
			case '\n':
				fprintf(output, "\\n");
				loop += 2;
				break;
			case '\t':
				fprintf(output, "\\t");
				loop += 2;
				break;
			default:
				fprintf(output, "%c", c);
				++loop;
				break;
		}

		if (loop > 70) {
			fprintf(output, "\"\n");
			loop = 0;
		}		

	} while(1);
}

const char *identify(const char *str)
{
	static char output[1024];
	int idx;

	strcpy(output, str);

	for (idx = 0; output[idx] != 0; ++idx) {
		if (output[idx] == '-')
			output[idx] = '_';
	}

	if (strcmp(output, "false") == 0)
		return "false_str";

	return output;
}

int main(int argc, char **argv)
{
	FILE *output;
	EmoField fieldId = 0;

	output = fopen("emofield.lua", "w");

	fprintf(output, "function protect_table (tbl)\n"
"return setmetatable ({}, \n"
"{\n"
"	__index = tbl,  \n"
"	__newindex = function (t, n, v)\n"
"	error (\"attempting to change constant \" .. \n"
"         tostring (n) .. \" to \" .. tostring (v), 2)\n"
"	end  \n"
"})\n"
"end\n");

	fprintf(output, "emo = { \n");

	for ( ; EmoFieldMap[fieldId].fieldId >= 0; ++fieldId)
		fprintf(output, "%s = %d,\n",
				identify(EmoFieldMap[fieldId].fieldName),
				EmoFieldMap[fieldId].fieldId);

	fprintf(output, "}\n");
	fprintf(output, "emo = protect_table(emo)\n");

	fclose(output);

	txt2inc("emofield.lua", "emofield.lua.inc", "emoinit");
}
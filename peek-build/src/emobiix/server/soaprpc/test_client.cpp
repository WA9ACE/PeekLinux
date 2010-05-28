#include "soap_request.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

int main()
{
	cerr << "Using \"bob\" pass \"torulethemall\" for authentication..." << endl;
	if (emobiix::soap_request::get_authentication("http://linux.emobiix.com:8082/cgi-bin/test.cgi", "bob", "torulethemall"))
		cerr << "Authentication successful!" << endl;

	string tree;
	if (emobiix::soap_request::get_treeDataObject("http://linux.emobiix.com:8082/cgi-bin/test.cgi", "sample", tree))
		cerr << "Tree data request successful: " << endl << tree << endl;

	vector<pair<size_t, unsigned char *> > blocks;
	if (emobiix::soap_request::get_blockDataObject("http://linux.emobiix.com:8082/cgi-bin/test.cgi", 1, blocks))
		cerr << "Block data request successful, got " << blocks.size() << " blocks of data" << endl;

	for (size_t i = 0; i < blocks.size(); ++i)
	{
		cerr << "Block " << i << ", size: " << blocks[i].first << endl;
		for (size_t j = 0; j < blocks[i].first; ++j)
		{
			cerr << hex << "0x" << setfill('0') << setw(2) << (int)blocks[i].second[j] << " ";
		}
		cerr << endl;
	}
}

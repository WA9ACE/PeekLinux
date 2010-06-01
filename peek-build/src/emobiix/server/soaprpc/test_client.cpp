#include "soap_request.h"
#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

int main()
{
	cerr << "Using \"bob\" pass \"torulethemall\" for authentication..." << endl;
	if (emobiix::soap_request::GetAuthentication("http://linux.emobiix.com:8082/cgi-bin/test.cgi", "1234567", "bob", "torulethemall"))
		cerr << "Authentication successful!" << endl;

	string tree;
	if (emobiix::soap_request::GetTreeDataObject("http://linux.emobiix.com:8082/cgi-bin/test.cgi", "sample", tree))
		cerr << "Tree data request successful: " << endl << tree << endl;

	string mime;
	vector<pair<size_t, unsigned char *> > blocks;
	if (emobiix::soap_request::GetBlockDataObject("http://linux.emobiix.com:8082/cgi-bin/test.cgi", "image.jpg", mime, blocks))
		cerr << "Block data request successful, got data of type " << mime << " of size " << blocks.size() << " blocks of data" << endl;

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

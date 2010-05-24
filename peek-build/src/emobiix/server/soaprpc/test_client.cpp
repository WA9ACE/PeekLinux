#include "soap_request.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
	cerr << "Using \"bob\" pass \"torulethemall\" for authentication..." << endl;
	if (emobiix::soap_request::get_authentication("http://linux.emobiix.com:8082/cgi-bin/test.cgi", "bob", "torulethemall"))
		cerr << "Authentication successful!" << endl;

	string tree;
	if (emobiix::soap_request::get_treeDataObject("http://linux.emobiix.com:8082/cgi-bin/test.cgi", "sample", tree))
		cerr << "Block data request successful: " << endl << tree << endl;
}

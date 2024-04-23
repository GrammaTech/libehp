/*
   Copyright 2017-2018 University of Virginia

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/


#include <ehp.hpp>
#include <iostream>
#include <assert.h>

using namespace std;
using namespace EHP;

void usage(int argc, char* argv[])
{
	cout<<"Usage: "<<argv[0]<<" <program to print eh info> "
		<<"[total-number-of-LSDA total-number-of-TypeTableEntries]\n";
	exit(1);
}



void print_lps(const EHFrameParser_t* ehp)
{
	const auto fdes=ehp->getFDEs();
	cout<<hex;
	for(const auto fde : *fdes)
	{
		cout<<"Found FDE at : " << fde->getStartAddress() << "-"<<fde->getEndAddress()<<endl;
		const auto lsda=fde->getLSDA();
		assert(lsda);
		const auto callsites=lsda->getCallSites();
		assert(callsites);

		for(const auto cs : *callsites)
		{
			cout<<"\tCall site (0x"<<cs->getCallSiteAddress()<<"-"<<cs->getCallSiteEndAddress()
			    <<") with landing pad=0x"<<cs->getLandingPadAddress()<<endl;
		}
	}
	cout<<dec;
}

int main(int argc, char* argv[])
{

	if(argc!=2 && argc!=4)
	{
		usage(argc,argv);
	}

	try
	{
		auto ehp = EHFrameParser_t::factory(argv[1]);
		ehp->print();


		print_lps(ehp.get());

		if (argc == 4) {
			int expected_lsda_count = atoi(argv[2]);
			int expected_tt_entry_count = atoi(argv[3]);

			int lsda_count = 0;
			int tt_entry_count = 0;
			for (const auto *fde : *(ehp->getFDEs()))
			{
				auto * lsda = fde->getLSDA();
				if (lsda && fde->getLSDAAddress() != 0)
				{
					lsda_count++;
					tt_entry_count += lsda->getTypeTable()->size();
				}
			}

			if (expected_lsda_count == lsda_count
				&& expected_tt_entry_count == tt_entry_count)
			{
				cout << "SUCCESS!\n"
					 << "  count(LSDA)=" << lsda_count << endl
					 << "  count(TypeTableEntries)=" << tt_entry_count << endl;
			}
			else
			{
				if (expected_lsda_count != lsda_count) {
					cout << "FAILED: count(LSDA)="
				 		 << lsda_count << " != " << expected_lsda_count << endl;
				}
				if (expected_tt_entry_count != tt_entry_count) {
					cout << "FAILED: count(TypeTableEntries)="
	 					 << tt_entry_count << " != " << expected_tt_entry_count
						 << endl;
				}
				return -1;
			}
		}
	}
	catch(const exception& e )
	{
		cout <<" libehp threw an exception, this may or may not be an error depending on the input file" << endl;
		cout << e.what() << endl;
	}
	catch(...)
	{
		cout <<" ehp threw an exception of an unknonwn type -- this shouldn't happen " << endl;
		abort();
	};

	return 0;
}

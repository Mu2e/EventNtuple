import os

class ruhelper:

    def list_available_cuts(self):
        with open(os.environ.get("EVENTNTUPLE_INC")+"/EventNtuple/utils/rooutil/inc/common_cuts.hh", 'r') as f:
            lines = f.readlines()
            for row in lines:
                if (row[:3] == "//+"):
                    print("\n"+row.replace('\n', ''))
                if ( (row.find("bool ") != -1) & (row.find("//") != -1) ):
                    print(row.replace('\n', ''))

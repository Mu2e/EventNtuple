# Import the Importer class
from pyimport import Importer 

# Initialise the Importer with increased verbosity 
# verbosity=0 will only show errors
importer = Importer(verbosity=2)

# Define the path to our example file
file_name = "/pnfs/mu2e/scratch/users/sophie/workflow/default/outstage/62418739/00/00000/nts.sophie.description.v0.001201_00000114.root"

branches = ["trksegs","event", "subrun", "run"]

# Import the branches from the file
# This loads the data into memory and returns an awkward array
data = importer.import_file(
    file_name=file_name,
    branches=branches
)


print(data["event"][0],data["subrun"][0],data["run"][0])



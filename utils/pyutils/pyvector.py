import uproot
import awkward as ak
import math
import numpy as np
import vector

class Vector:
  
  def __init__(self):
    """  Placeholder init """ 
    pass  
    
  def GetVectorXYZ(self, trksid, leafname, vectorreq):
    """ 
    imports a XYZ vector branch e.g. mom and turns it into something which can use funcitons are found in:
    https://vector.readthedocs.io/en/latest/api/vector._methods.html 
    """

    # register the vector class
    vector.register_awkward()

    # make the Vector 3D TODO - protect against str(leafname)  doesnt exist error
    trkvect3D = ak.zip({
        "x": trksid[str(leafname)][str(vectorreq)]["fCoordinates"]["fX"],
        "y": trksid[str(leafname)][str(vectorreq)]["fCoordinates"]["fY"],
        "z": trksid[str(leafname)][str(vectorreq)]["fCoordinates"]["fZ"],
    }, with_name="Vector3D")
    
    return trkvect3D
  
  def Mag(self, vector):
    """ returns magnitude of vector """
    mag = vector.mag
    return mag
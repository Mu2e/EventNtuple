#! /usr/bin/env python
import uproot
import awkward as ak
import numpy as np
from pylogger import Logger
    
class Select:
    """
    Class for standard selection cuts with EventNtuple data in Awkward format
    
    """
    def __init__(self, verbosity=1):
        """Initialise the selector
        
            Args: 
                verbosity (int, optional): Print detail level (0: minimal, 1: medium, 2: maximum). Defaults to 1. 
        """
        # Start logger
        self.logger = Logger( 
            print_prefix = "[pyselect]", 
            verbosity = verbosity
        )
        # PDG ID dict
        self.particles = { 
            "e-" : 11,
            "e+" : -11,
            "mu-" : 13,
            "mu+" : -13
        }

    def is_electron(self, data):
        """ Return boolean array for electron tracks which can be used as a mask 

            Args:
                data (awkward.Array): Input array containing the "trk" branch
        """
        try:
            # Construct & return mask
            mask = (data["trk.pdg"] == self.particles["e-"])
            self.logger.log(f"Returning mask for e- tracks", "success")
            return mask
        except Exception as e:
            self.logger.log(f"Exception in is_electron(): {e}", "error")
            return None
            
    def is_positron(self, data):
        """ Return boolean array for positron tracks which can be used as a mask 

            Args:
                data (awkward.Array): Input array containing the "trk" branch
        """
        try:
            # Construct & return mask
            mask = (data["trk.pdg"] == self.particles["e+"])
            self.logger.log(f"Returning mask for e+ tracks", "success")
            return mask
        except Exception as e:
            self.logger.log(f"Exception in is_positron(): {e}", "error")
            return None
            
    def is_mu_minus(self, data):
        """ Return boolean array for negative muon tracks which can be used as a mask 

            Args:
                data (awkward.Array): Input array containing the "trk" branch
        """
        try:
            # Construct & return mask
            mask = (data["trk.pdg"] == self.particles["mu-"]) 
            self.logger.log(f"Returning mask for mu- tracks", "success")
            return mask
        except Exception as e:
            self.logger.log(f"Exception in is_mu_minus(): {e}", "error")
            return None

    def is_mu_plus(self, data):
        """ Return boolean array for positive muon tracks which can be used as a mask 

            Args:
                data (awkward.Array): Input array containing the "trk" branch
        """
        try:
            # Construct & return mask
            mask = (data["trk.pdg"] == self.particles["mu+"])
            self.logger.log(f"Returning mask for mu+ tracks", "success")
            return mask 
        except Exception as e:
            self.logger.log(f"Exception in is_mu_plus(): {e}", "error")
            return None

    # More general function for particle selection
    def is_particle(self, data, particle):
        """ Return boolean array for tracks of a specific particle type which can be used as a mask 

            Args:
                data (awkward.Array): Input array containing the "trk" branch
                particle (string): particle type, 'e-', 'e+', 'mu-', or 'mu+'
        """
        # "trk" is not the only branch that uses pdgIDs, crvcoincsmc is another, how to handle that? 
        try:
            # Construct & return mask
            mask = (data["trk.pdg"] == self.particles[particle])
            self.logger.log(f"Returning mask for {particle} tracks", "success")
            return mask
        except Exception as e:
            self.logger.log(f"Exception in is_particle(): {e}", "error")
            return None
            
    def is_downstream(self, data, branch_name="trksegs"):
        """ Return boolean array for upstream track segments

            Args:
                data (awkward.Array): Input array containing the segments branch
                branch_name (str, optional): Name of the segments branch for backwards compatibility. Defaults to 'trksegs'
        """
        try:
            # Construct & return mask
            mask = (data[branch_name]["mom"]["fCoordinates"]["fZ"] > 0)
            self.logger.log(f"Returning mask for downstream track segments (p_z > 0)", "success")
            return mask
        except Exception as e:
            self.logger.log(f"Exception in is_downstream(): {e}", "error")
            return None

    def is_upstream(self, data, branch_name="trksegs"):
        """ Return boolean array for downstream track segments 

            Args:
                data (awkward.Array): Input array containing the segments branch
                branch_name (str, optional): Name of the segments branch for backwards compatibility. Defaults to 'trksegs'
        """
        try:
            # Construct & return mask
             mask = (data["trksegs"]["mom"]["fCoordinates"]["fZ"] < 0)
             self.logger.log(f"Returning mask for upstream track segments (p_z < 0)", "success")
             return mask
        except Exception as e:
            self.logger.log(f"Exception in is_upstream(): {e}", "error")
            return None

    def select_surface(self, data, sid, sindex=0, branch_name="trksegs"):
        """ Return boolean array for track segments intersecting a specific surface 
        
        Args:
            data (awkward.Array): Input array containing segments branch
            sid (int): ID of the intersected surface 
            sindex (int, optional): Index to the intersected surface (for multi-surface elements). Defaults to 0. 
            branch_name (str, optional): Name of the segments branch for backwards compatibility. Defaults to 'trksegs'
        """
        try:
            # Construct & return mask
            mask = (data[branch_name]['sid']==sid) & (data[branch_name]['sindex']==sindex)
            self.logger.log(f"Returning mask for {branch_name} with sid = {sid} and sindex = {sindex}", "success")
            return mask
        except Exception as e:
            self.logger.log(f"Exception in select_surface(): {e}", "error")
            return None

    def is_reflected(self, data, branch_name="trksegs"):
        """ Return boolean array for reflected tracks  
        
        Reflected tracks have both upstream and downstream segments at the tracker entrance 
        
        Args:
            data (awkward.Array): Input array containing segments branch
            branch_name (str, optional): Name of the segments branch for backwards compatibility. Defaults to 'trksegs'
        """
        try:
            # Construct track segment conditions
            trkent = self.select_surface(data, sid=0, branch_name=branch_name)
            upstream = self.is_upstream(data, branch_name=branch_name)
            downstream = self.is_downstream(data, branch_name=branch_name)
            # Construct condition for reflected tracks 
            # Does the track have any segments with p_z > 0 at the tracker entrance 
            # AND at any segments with p_z < 0 at the tracker entrance?
            reflected = (ak.any((upstream & trkent), axis=-1) & ak.any((downstream & trkent), axis=-1))
            self.logger.log(f"Returning mask for reflected tracks", "success")
            return reflected
        except Exception as e:
            self.logger.log(f"Exception in is_reflected(): {e}", "error")
            return None
            
    def select_trkqual(self, data, quality):
        """ Return boolean array for tracks above a specified quality   

        Args: 
            data (awkward.Array): Input array containing the trkqual.result branch
            quality (float): The numerical output of the MVA

        """
        try:
            # Construct & return mask
            mask = (data["trkqual.result"] > quality)
            self.logger.log(f"Returning mask for trkqual > {quality}", "success")
            return mask
        except Exception as e:
            self.logger.log(f"Exception in select_trkqual(): {e}", "error")
            return None
            
    def has_n_hits(self, data, n_hits):
        """ Return boolean array for tracks with hits above a specified value 

        Hits in this context is nactive planes

        Args: 
            data (awkward.Array): Input array containing the trk.nactive branch
            n_hits (int): The minimum number of track hits (nactive)

        """
        try:
            # Construct & return mask
            # Why nactive and not nhits?
            mask = (data["trk.nactive"] >= n_hits)
            self.logger.log(f"Returning mask for nactive > {n_hits}", "success")
            return mask
        except Exception as e:
            self.logger.log(f"Exception in has_n_hits(): {e}", "error")
            return None

    # Below code may need a review? 
    
    def hasTrkCrvCoincs(self, trks, ntuple, tmax):
        """ simple function to remove anything close to a crv coinc """

        # Looping through events like this is pretty inefficient 
        # Can we do it with array operations? 
        # Can we explain a bit more about what this does?
        
        crvst = ntuple["crvcoincs"]
        crvs = crvst.arrays(library='ak')
        has_coin = np.ones(ak.num(trks, axis=0), dtype=bool)
        for i_evt, evt in enumerate(trks['trksegs']['time']):
            for i_trk, trk in enumerate(evt):
                if ak.num(ak.drop_none(trk), axis = 0) > 0:
                    for i_crv, crv in enumerate(crvs['crvcoincs.time'][ i_evt]):
                        if np.abs(trk[0] - crv) < tmax:
                            has_coin[i_evt] = False
        return has_coin


    def MakeMask(self, branch, treename, leaf, eql, v1, v2=None):
        """ makes a mask for the chosen branch/leaf v1 = min, v2 = max, use eql if you want it == v1"""

        # Can we explain a bit more about what this does?
        
        mask=""
        if eql == True:
            mask = (branch[str(treename)][str(leaf)]==v1)
        else:
            mask = (branch[str(treename)][str(leaf)] >  v1) & (branch[str(treename)][str(leaf)] < v2)
        return mask

    def MakeMaskList(self, branch, treenames, leaves, eqs, v1s, v2s):
        """ makes a mask for the chosen branch/leaf v1 = min, v2 = max, use eql if you want it == v1"""

        # Can we explain a bit more about what this does?
        
        mask_list=[]
        print(treenames,leaves,eqs,v1s,v2s)
        for i, tree in enumerate(treenames):
            print(treenames[i],leaves[i],eqs[i],v1s[i],v2s[i])
            mask = ""
            if eqs[i] == True:
                mask = (branch[str(treenames[i])][str(leaves[i])] == v1s[i])
                print(i, mask)
            else:
                mask = (branch[str(treenames[i])][str(leaves[i])] >  v1s[i]) & (branch[str(treenames[i])][str(leaves[i])] < v2s[i])
                print(i, mask)
            mask_list.append(mask)
        return mask_list
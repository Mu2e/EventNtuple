import os

class nthelper:

    single_object_branches = ['evtinfo', 'evtinfomc', 'hitcount', 'tcnt', 'crvsummary', 'crvsummarymc']
    vector_object_branches = ['trk', 'trkmc', 'trkcalohit', 'trkcalohitmc', 'crvcoincs', 'crvcoincsmc', 'crvcoincsmcplane', 'trkqual']
    vector_vector_object_branches = ['trksegs', 'trksegpars_lh', 'trksegpars_ch', 'trksegpars_kl', 'trkmcsim', 'trkhits', 'trkhitsmc', 'trkmats', 'trkmcsci', 'trkmcssi', 'trksegsmc' ]

    evt_branches = ['evtinfo','evtinfomc','hitcount','tcnt']
    trk_branches = ['trk', 'trkmc', 'trkcalohit', 'trkqual']
    trksegs_branches = ['trksegs', 'trksegpars_lh', 'trksegpars_ch', 'trksegpars_kl', 'trksegsmc']
    straw_branches = ['trkhits', 'trkmats', 'trkhitsmc']
    mc_branches = ['trkmcsim']
    crv_branches = ['crvsummary','crvsummarymc','crvcoincs','crvcoincsmc','crvcoincsmcplane']
    unknown_branches = ['trkcalohitmc','trkmcsci','trkmcssi']

    track_types_dict = { 'kl' : "kinematic line fit (i.e. straight-line fit)",
                         'dem' : "downstream e-minus fit",
                         'uem' : "upstream e-minus fit",
                         'dmm' : "downstream mu-minus fit",
                         'umm' : "upstream mu-minus fit",
                         'dep' : "downstream e-plus fit",
                         'uep' : "upstream e-plus fit",
                         'dmp' : "downstream mu-plus fit",
                         'ump' : "upstream mu-plus fit"
                        }

    # A dictionary of branch name to header file containing the struct
    branch_struct_dict = { 'evtinfo' : "EventInfo",
                           'evtinfomc' : "EventInfoMC",
                           'hitcount' : "HitCount",
                           'tcnt' : "TrkCount", # TODO: leaves can't be retrieved because they are runtime made
                           'trk' : "TrkInfo",
                           'trksegs' : "TrkSegInfo",
                           'trkmc' : "TrkInfoMC",
                           'trksegpars_lh' : "LoopHelixInfo",
                           'trksegpars_ch' : "CentralHelixInfo",
                           'trksegpars_kl' : "KinematicLineInfo",
                           'trkmcsim' : "SimInfo",
                           'trkcalohit' : "TrkCaloHitInfo",
                           'trkcalohitmc' : "CaloClusterInfoMC",
                           'trkhits' : "TrkStrawHitInfo",
                           'trkhitsmc' : "TrkStrawHitInfoMC",
                           'trkmats' : "TrkStrawMatInfo",
                           'trkmcsci' : "MCStepInfo",
                           'trkmcssi' : "MCStepSummaryInfo",
                           "crvsummary" : "CrvSummaryReco",
                           "crvsummarymc" : "CrvSummaryMC",
                           "crvcoincs" : "CrvHitInfoReco",
                           "crvcoincsmc" : "CrvHitInfoMC",
                           "crvcoincsmcplane" : "CrvPlaneInfoMC",
                           "trkqual" : "MVAResultInfo",
                           "helices" : "HelixInfo",
                           "trksegsmc" : "SurfaceStepInfo"
                          }

    #
    def get_branch_explanation(self, branch):
        explanation=""
        if branch in self.track_types_dict.keys():
            explanation = "This is an outdated (v5) branch."
        else:
            structure=""
            if branch in self.single_object_branches:
                structure = "Single objects branch"
            elif branch in self.vector_object_branches:
                structure = "Vector branch"
            elif branch in self.vector_vector_object_branches:
                structure = "Vector-of-vector branch"
            explanation = branch + ": " + structure + ": ";
            struct = self.branch_struct_dict[branch]
            struct_file = struct + ".hh";
            with open(os.environ.get("EVENTNTUPLE_INC")+"/EventNtuple/inc/"+struct_file, 'r') as f:
                lines = f.readlines()
                for row in lines:
                    if (row.find("// "+struct) != -1):
                        explanation += row.replace("// "+struct+":", "").replace('\n', ''); # remove the trailing newline as well

        return explanation

    def list_all_branches(self, export_to_md=False):
        if not export_to_md:
            print("Event Branches")
            print("======================")
        else:
            print("Note this page is autogenerated with ```ntuplehelper --list-all-branches --export-to-md > doc/branches.md```\n")
            print("## Event Branches\n")
            print("These branches contain one element per event\n")
            print("| branch | structure | explanation | leaf information |")
            print("|--------|-----------|-------------|------------------|")

        for branch in self.evt_branches:
            explanation = self.get_branch_explanation(branch)
            struct = self.branch_struct_dict[branch]
            struct_file = struct + ".hh";
            if not export_to_md:
                print(explanation)
            else:
                tokens=explanation.split(":")
                print("| " + tokens[0] + " | " + tokens[1] + " | " + tokens[2] + "| [see " + struct_file + "](../inc/"+struct_file+")")

        if not export_to_md:
            print("\nTrack Branches")
            print("================")
        else:
            print("## Track Branches\n")
            print("These branches contain 4 elements per event corresponding to different Kalman fit hypotheses:")
            print("- downstream electron")
            print("- downstream positron")
            print("- downstream negative muon")
            print("- downstream positive muon")
            print("In case the track is reflected and have an upstream leg and a downstream leg, then each event contains 8 elements")
            print("The last 4 elements are upstream equivalent to the 4 elements listed above\n")
            print("| branch | structure | explanation | leaf information |")
            print("|--------|-----------|-------------|------------------|")
        for branch in self.trk_branches:
            explanation = self.get_branch_explanation(branch)
            struct = self.branch_struct_dict[branch]
            struct_file = struct + ".hh";
            if not export_to_md:
                print(explanation)
            else:
                tokens=explanation.split(":")
                print("| " + tokens[0] + " | " + tokens[1] + " | " + tokens[2] + "| [see " + struct_file + "](../inc/"+struct_file+")")

        if not export_to_md:
            print("\nTrack segments Branches")
            print("================")
        else:
            print("## Track segments Branches\n")
            print("These branches contain 4 elements per event corresponding to different Kalman fit hypotheses (see Track branches)")
            print("Within each Kalman fit element, there is a vector describing the track segment parameters when intersecting virtual detectors (identified by their surface id sid)\n")
            print("| branch | structure | explanation | leaf information |")
            print("|--------|-----------|-------------|------------------|")
        for branch in self.trksegs_branches:
            explanation = self.get_branch_explanation(branch)
            struct = self.branch_struct_dict[branch]
            struct_file = struct + ".hh";
            if not export_to_md:
                print(explanation)
            else:
                tokens=explanation.split(":")
                print("| " + tokens[0] + " | " + tokens[1] + " | " + tokens[2] + "| [see " + struct_file + "](../inc/"+struct_file+")")

        if not export_to_md:
            print("\nStraw Branches")
            print("================")
        else:
            print("## Straw Branches\n")
            print("These branches contain a vector in which each element lists the properties of a straw hits in the tracker")
            print("The number of elements in the vector is found in the leaf trk.nhits\n")
            print("| branch | structure | explanation | leaf information |")
            print("|--------|-----------|-------------|------------------|")
        for branch in self.straw_branches:
            explanation = self.get_branch_explanation(branch)
            struct = self.branch_struct_dict[branch]
            struct_file = struct + ".hh";
            if not export_to_md:
                print(explanation)
            else:
                tokens=explanation.split(":")
                print("| " + tokens[0] + " | " + tokens[1] + " | " + tokens[2] + "| [see " + struct_file + "](../inc/"+struct_file+")")

        if not export_to_md:
            print("\nMonte Carlo Branches")
            print("================")
        else:
            print("## Monte Carlo Branches\n")
            print("These branches contain 4 elements per event corresponding to different Kalman fit hypotheses (see Track branches)")
            print("Within each Kalman fit element, there is a vector containing Monte Carlo truth information about the particle leaving the track and its parent particles")
            print("The vector is sorted in reverse chronological order, such that the last element is the initial particle simulated in GEANT4, and each element before correspond to one of its daughter particles\n")
            print("| branch | structure | explanation | leaf information |")
            print("|--------|-----------|-------------|------------------|")
        for branch in self.mc_branches:
            explanation = self.get_branch_explanation(branch)
            struct = self.branch_struct_dict[branch]
            struct_file = struct + ".hh";
            if not export_to_md:
                print(explanation)
            else:
                tokens=explanation.split(":")
                print("| " + tokens[0] + " | " + tokens[1] + " | " + tokens[2] + "| [see " + struct_file + "](../inc/"+struct_file+")")

        if not export_to_md:
            print("\nCRV Branches")
            print("================")
        else:
            print("## CRV Branches\n")
            print("These branches contain a vector where each element is a CRV hit that happened during the event")
            print("The branch is empty if there are no CRV hit during the event\n")
            print("| branch | structure | explanation | leaf information |")
            print("|--------|-----------|-------------|------------------|")
        for branch in self.crv_branches:
            explanation = self.get_branch_explanation(branch)
            struct = self.branch_struct_dict[branch]
            struct_file = struct + ".hh";
            if not export_to_md:
                print(explanation)
            else:
                tokens=explanation.split(":")
                print("| " + tokens[0] + " | " + tokens[1] + " | " + tokens[2] + "| [see " + struct_file + "](../inc/"+struct_file+")")

        if not export_to_md:
            print("\n\nTo get all leaf information on any of the branches above run \"ntuplehelper branch.*\" (For example: \"ntuplehelper trk.*\")")


    def whatis(self, array):
        if type(array) is not list: # if a single string is passed, put it into an array
            array = [array]

        # Let's collect leaves from the same branch so that we don't repeat information
        branch_leaves_dict = {}
        for item in array:
            # Expecting "item" to be of form "branch.leaf"
            tokens = item.split('.')
            branch = tokens[0]
            leaf = ""
            if len(tokens)>1:
                leaf = tokens[1]
            try:
                branch_leaves_dict[branch].append(leaf)
            except KeyError:
                branch_leaves_dict[branch] = [leaf]

        for i_branch, leaves in branch_leaves_dict.items():
#            print(i_branch, leaves)

            # Check if this is a track branch
            branch_explanation = self.get_branch_explanation(i_branch)
            leaf_output = "";
            leaf_explanations = {};
            try:
                struct = self.branch_struct_dict[i_branch]
                struct_file = struct;
                if (".hh" not in struct_file):
                    struct_file += ".hh"

#                print(struct_file)
                with open(os.environ.get("EVENTNTUPLE_INC")+"/EventNtuple/inc/"+struct_file, 'r') as f:
                    lines = f.readlines()
                    for row in lines:
                        # Check whether this row is an explanation for a leaf that we are asking for
                        for i_leaf in leaves:
                            if i_leaf == "*":
                                if (row.find(" = ") != -1) and (row.find(";") != -1) and (row.find("//") != -1):
                                    if i_leaf not in leaf_explanations: # we want to only take the first occurance
                                        leaf_explanations[i_leaf] = "\n"+row
                                    else:
                                        leaf_explanations[i_leaf] += row;
                            else:
                                if (row.find(" "+i_leaf+" ") != -1) or (row.find(" "+i_leaf+";") != -1): # add spaces around leaf so that we don't find substrings
                                    if i_leaf not in leaf_explanations: # we want to only take the first occurance
                                        leaf_explanations[i_leaf] = row
            except KeyError:
                print(i_branch+" is not in branch_struct_dict...\n")

            # Check that we have all the explanations...
            for i_leaf in leaves:
                if i_leaf not in leaf_explanations:
                    leaf_explanations[i_leaf] = "not found\n";
                # ... and produce the output text
                leaf_output += i_branch + "." + i_leaf + ": "+leaf_explanations[i_leaf];

            # Display the output text
            print(branch_explanation)
            print(leaf_output)

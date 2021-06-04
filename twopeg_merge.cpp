#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include <iostream>
#include "functions.hpp"

/*
Written by Chris McLauchlin: cmc@jlab.org
This is meant to take in the simulation and weight files 
created by Dr. Iulia S's TWOPEG event generator and subsequent simulation
Weights are stored in a separate file due to bosfile structures
This lines up weights with the proper events by their thrown electron momentum

./twopeg_merge <reconstructed file> <weight file>
*/


//Main body
int main(int argc, char **argv){
	std::cout<<"Begin Merger of ";

	//Take in the output sim file and the corresponding weight file
	char* sim = argv[1]; 
    char* weight = argv[2];
    std::cout<<sim <<" and " <<weight <<std::endl;

    Float_t ele_p_sim[4];
    int sim_idx = 0; 
    int num_sim = 0;
    int num_thr = 0; 
    bool end = false;


    std::cout<<"Read Weight Tree" <<std::endl;
    //Create file and tree for the weights
	TFile *file2 = TFile::Open(weight,"READ");
	TTree *T2 = (TTree*)file2->Get("h10");
	std::cout<<"Assign Weight Branches" <<std::endl;
	//Create Branches for intermediary Tree
	TBranch *th_sig = T2->GetBranch("sigma");
	TBranch *th_mom = T2->GetBranch("p_el_test");

	std::cout<<"Update sim Tree" <<std::endl;
	//Get the simulation file and create a tree for it
	TFile *file1 = TFile::Open(sim,"READ");
	TTree *T1 = (TTree*)file1->Get("h10");
	


	//Create new Merged Tree
	std::string merge_name = argv[1];
	fun::replace(merge_name,".root","_merged.root");//create new name for merged file
	std::cout<<"Merge File name: " <<merge_name <<std::endl;//Check on merged file to be made
	TFile *file_merged = new TFile((merge_name).c_str(),"RECREATE");
	auto T0 = T1->CloneTree();
	std::cout<<"Sim branches on new Tree" <<std::endl;
	//Create New Branches in addition to what was there previous
	Float_t sigma_tot;
	TBranch *sigma_total = T0->Branch("weight", &sigma_tot, "weight/F");//Event weight
	T0-> SetBranchAddress("mcp",&ele_p_sim);//Thrown electron momentum
	

	num_sim = T0->GetEntries();
	num_thr = T2->GetEntries();
	//Compare reconstucted events to thrown events
	std::cout<<"num of recon events: " <<num_sim <<std::endl;
	std::cout<<"num of thrown events: " <<num_thr <<std::endl;



	//Fill the branches on this new Tree
	for (int i=0; i<num_thr; i++) { 
		//Get the pieces from the weight
		th_sig-> GetEntry(i);
		th_mom-> GetEntry(i);
		if(sim_idx < num_sim){
			std::cout<<"sim_idx: " <<sim_idx <<"| num_sim: " <<num_sim <<"\r" <<std::flush; 
			if(i-sim_idx <= num_thr - num_sim){
				T0->GetEntry(sim_idx); 
				if(ele_p_sim[0] == th_mom->GetLeaf("p_el_test")->GetValue()){
					sigma_tot = th_sig->GetLeaf("sigma")->GetValue();

					//Fill the new sim branches with the weights
					sigma_total->Fill();
					sim_idx = sim_idx + 1;
				}else{
					std::cout<<"It's not a match| thr idx:	" <<i <<"	| sim idx:	" <<sim_idx <<std::endl;
					std::cout<<"thrown p idx(" <<i <<"):	" <<th_mom->GetLeaf("p_el_test")->GetValue() <<"	| sim thrown p (" <<sim_idx <<"):	" <<ele_p_sim[0] <<std::endl;
				}
			}else{
				sim_idx = sim_idx + 1; 
				i = i - (1+num_thr-num_sim);
			}
		}else if(!end){
			std::cout<<"Exceeded sim events" <<std::endl;
			end = true; 
		}
	}
	//Overwrite the old sim file with the additional weight branches
	std::cout<<"Writing New Merged TTree" <<std::endl;
	file_merged->cd();
	T0->Write("", TObject::kOverwrite);
	std::cout<<"Complete" <<std::endl;
	file_merged->Close();
	
	return 0; 
}

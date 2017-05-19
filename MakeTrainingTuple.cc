#include <fstream>
#include <iostream>
#include <vector>
#include <TROOT.h>
#include <TLatex.h>
#include <TFile.h>
#include <TTree.h>
#include <TChainElement.h>
#include <sstream>
#include <TChain.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TBranch.h>
#include <TLorentzVector.h>
#include <iomanip>
#include <TH1F.h>
#include <TLeaf.h>
#include <TClassRef.h>
#include <TEntryListArray.h>
#include "/afs/cern.ch/user/b/bkhanji/roofitinclude.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#include "TEfficiency.h"


void DefineImportantVariables(TTree* mytree = 0 )
{
  
  mytree->SetBranchStatus("*",0);
  mytree->SetBranchStatus("Tr_T_*",1);
  mytree->SetBranchStatus("B_ID",1);
  mytree->SetBranchStatus("B_LOKI_MASS_JpsiConstr",1);
  mytree->SetBranchStatus("B_*_PROB",1);
  mytree->SetBranchStatus("B_*_DEC",1);
  mytree->SetBranchStatus("B_TAGDECISION_OS",1);
  mytree->SetBranchStatus("B_TAGOMEGA_OS",1);
  //mytree->SetBranchStatus("*ProbNN*",1);
  mytree->SetBranchStatus("B_M",1);
  mytree->SetBranchStatus("SigYield_sw",1);
  mytree->SetBranchStatus("B_LOKI_DTF_CTAUS",1);
  mytree->SetBranchStatus("B_TAU",1);
  mytree->SetBranchStatus("B_LOKI_PHI",1);
  mytree->SetBranchStatus("B_LOKI_ETA",1);
  mytree->SetBranchStatus("B_P",1);
  mytree->SetBranchStatus("B_PT",1);
  mytree->SetBranchStatus("nTracks",1);
  mytree->SetBranchStatus("nPV",1);
  mytree->SetBranchStatus("*Number",1);
  
  return ;
}

/*
bool SelectProtonTagger(int i = 0 )
{
  bool Pass_sel = false;
  //
  
  {
  Pass_sel = true;
  }
  
  return Pass_sel;
}
*/

 //Year        = [ "2011","2012" ,"2015","2016" ]
 //Channel     = [ "Bd2JpsiKstar","Bu2JpsiK"    ]

Bool_t verbose = false ;

int MakeTrainingTuple( TString year = "2011" , TString channel ="Bu2JpsiK" )
{

  TString Suffix_year   = year + "*" + channel ;
  cout<<"======================================================"<<endl;
  cout<<"======================================================"<<endl;
  cout<< "making the training tuples for : " <<channel <<" channel , " << year << " in data"<<endl;
  cout<<"======================================================"<<endl;
  cout<<"======================================================"<<endl;
  // run 
  TString eos_dir = "/eos/lhcb/wg/FlavourTagging/tuples/development/data/B2CC/20170223_vmueller/";
  TString WS_dir  = "/afs/cern.ch/work/b/bkhanji/public/Tuples/"+ channel +"/FlatTrees/";
  cout<<"++++++++++++++++++++++++++++++++++++++++++++"<<endl;
  TChain *mychain = new TChain("","");
  mychain->Add(eos_dir + "*"+ Suffix_year +"_sW.root/DecayTree");
  cout<< " Number of entries found: "<< mychain->GetEntries()<<endl;
  cout<<" Tuples are : "<<endl;
  TIter N_iter(mychain->GetListOfFiles());
  cout<<"======================================================"<<endl;
  while (TChainElement *obj = (TChainElement*)N_iter()  )
  { TFile f(obj->GetTitle());  cout << f.GetName() << endl; }
  cout<<"======================================================"<<endl;
    
  DefineImportantVariables(mychain);
  TTree *mychain_c = mychain->CloneTree(0);
  
  //cout<< " Size of List_of_BranchValues : " << mychain_c->GetListOfBranches()->GetEntries()<<endl;
  
  TFile newfile(WS_dir + "osproton_"+ year +"_"+ channel  +"_Tight.root","RECREATE");
  TTree FTT_tree("FlatTrackTree","FlatTrackTree");
  
  TObjArray *obj_list_br = mychain_c->GetListOfBranches();
  int n_vars = obj_list_br->GetEntries();
  Float_t var_branch_flat[n_vars];
  
  Int_t var_branch_flat_int[n_vars];
  TH1D *mutliplicit = new TH1D("mutliplicit","mutliplicit" , 100 , 0. , 20);
  
  for(int i= 0 ; i < n_vars ; i++)
  {
    TString Var_br_name  = ((TBranch*)(*obj_list_br)[i])->GetName();
    TBranch *var_branch ;
    
    if (Var_br_name.Contains("_DEC" ) || Var_br_name.Contains("B_ID" ) || Var_br_name.Contains("DECISION" ) )
    {
      //cout<<" I found DECISON variable "<< endl;
      //cout<<" Variable is "<< Var_br_name << endl;
      var_branch  = FTT_tree.Branch( Var_br_name , &var_branch_flat_int[i] , Var_br_name + (TString)"/I");
    }
    else{
      var_branch = FTT_tree.Branch( Var_br_name , &var_branch_flat[i] , Var_br_name + (TString)"/F");  
    }
    
    if (verbose) cout <<" variable name : " << Var_br_name <<endl;
  }
  
  Int_t B_OSp_DECISION;
  Float_t Tr_T_P_FROM_Lb_PROB;
  // TBranch *B_OSp_DECISION_b = 
  FTT_tree.Branch( "B_OS_Proton_DEC" , &B_OSp_DECISION , "B_OS_Proton_DEC/I" );
  FTT_tree.Branch( "Tr_T_P_FROM_Lb_PROB" , &Tr_T_P_FROM_Lb_PROB , "Tr_T_P_FROM_Lb_PROB/F" );
  
  Int_t N ;
  Float_t Tr_T_PT[1000], Tr_T_PROBNNp[1000],  Tr_T_PROBNNk[1000] , Tr_T_BPVIPCHI2[1000], Tr_T_PROBNNpi[1000] , 
    Tr_T_P[1000] , Tr_T_TRGHOSTPROB[1000] , Tr_T_PIDp[1000] , Tr_T_PIDK[1000] , Tr_T_BPVIP[1000] 
    , Tr_T_MinIPChi2[1000] , Tr_T_Charge[1000] , Tr_T_TRTYPE[1000] , Tr_T_zfirst[1000] ,
    Tr_T_ETA[1000],Tr_T_Phi[1000],Tr_T_PROBNNmu[1000],Tr_T_PIDmu[1000],Tr_T_MinIP[1000],Tr_T_HcalE[1000] , 
    Tr_T_nBDT_ult[1000],Tr_T_SumBDT_ult[1000],Tr_T_Ntr_incone[1000],Tr_T_Cone_asym_P[1000] , Tr_T_ConIso_pt_ult[1000];
  
  mychain->SetBranchAddress( "N" , &N);
  mychain->SetBranchAddress( "Tr_T_Charge", &Tr_T_Charge);
  mychain->SetBranchAddress( "Tr_T_PT" , &Tr_T_PT );
  mychain->SetBranchAddress( "Tr_T_PROBNNp" , &Tr_T_PROBNNp);
  mychain->SetBranchAddress( "Tr_T_PROBNNk" , &Tr_T_PROBNNk);
  mychain->SetBranchAddress( "Tr_T_PROBNNpi" , &Tr_T_PROBNNpi);
  mychain->SetBranchAddress( "Tr_T_BPVIPCHI2" , &Tr_T_BPVIPCHI2);
  mychain->SetBranchAddress( "Tr_T_BPVIP" , &Tr_T_BPVIP);
  mychain->SetBranchAddress( "Tr_T_P" , &Tr_T_P);
  mychain->SetBranchAddress( "Tr_T_PIDp" , &Tr_T_PIDp);
  mychain->SetBranchAddress( "Tr_T_PIDK" , &Tr_T_PIDK);
  mychain->SetBranchAddress( "Tr_T_MinIPChi2" , &Tr_T_MinIPChi2);
  mychain->SetBranchAddress( "Tr_T_TRTYPE"  , &Tr_T_TRTYPE);
  mychain->SetBranchAddress( "Tr_T_zfirst"  , &Tr_T_zfirst);
  mychain->SetBranchAddress( "Tr_T_Eta"    , &Tr_T_ETA);
  mychain->SetBranchAddress( "Tr_T_Phi"     , &Tr_T_Phi);
  mychain->SetBranchAddress( "Tr_T_PROBNNmu", &Tr_T_PROBNNmu);
  mychain->SetBranchAddress( "Tr_T_PIDmu"   , &Tr_T_PIDmu);
  mychain->SetBranchAddress( "Tr_T_MinIP"   , &Tr_T_MinIP);
  mychain->SetBranchAddress( "Tr_T_HcalE"   , &Tr_T_HcalE);
  mychain->SetBranchAddress( "Tr_T_nBDT_ult"     ,  &Tr_T_nBDT_ult);    
  mychain->SetBranchAddress( "Tr_T_SumBDT_ult"   ,  &Tr_T_SumBDT_ult);  
  mychain->SetBranchAddress( "Tr_T_Ntr_incone"   ,  &Tr_T_Ntr_incone);  
  mychain->SetBranchAddress( "Tr_T_Cone_asym_P"  ,  &Tr_T_Cone_asym_P);        
  mychain->SetBranchAddress( "Tr_T_ConIso_pt_ult"  ,  &Tr_T_ConIso_pt_ult);        
  TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );
  Float_t R_Tr_T_zfirst,R_Tr_T_ETA,R_Tr_T_Phi,R_Tr_T_PROBNNpi,
    R_Tr_T_PROBNNppi,R_Tr_T_PROBNNmu,R_Tr_T_PROBNNpk, R_Tr_T_PROBNNk,
    R_Tr_T_PIDp,R_Tr_T_PIDmu,R_Tr_T_PIDK,
    R_Tr_T_P,R_Tr_T_MinIPCHI2,R_Tr_T_HcalE, R_Tr_T_BPVIP  ,R_Tr_T_nBDT_ult , 
    R_Tr_T_SumBDT_ult,R_Tr_T_Ntr_incone,R_Tr_T_Cone_asym_P, R_Tr_T_ConIso_pt_ult;
  
  reader->AddVariable( "Tr_T_zfirst"       ,  &R_Tr_T_zfirst);      
  reader->AddVariable( "Tr_T_Eta"          ,  &R_Tr_T_ETA);         
  reader->AddVariable( "Tr_T_Phi"          ,  &R_Tr_T_Phi);         
  reader->AddVariable( "Tr_T_PROBNNp*(1.-Tr_T_PROBNNpi)"    ,  &R_Tr_T_PROBNNppi);    
  reader->AddVariable( "Tr_T_PROBNNp*(1.-Tr_T_PROBNNk)"     ,  &R_Tr_T_PROBNNpk);     
  reader->AddVariable( "Tr_T_PROBNNk"      ,  &R_Tr_T_PROBNNk);     
  reader->AddVariable( "Tr_T_PIDp"         ,  &R_Tr_T_PIDp);        
  reader->AddVariable( "Tr_T_PIDmu"        ,  &R_Tr_T_PIDmu);       
  reader->AddVariable( "Tr_T_PIDK"         ,  &R_Tr_T_PIDK);        
  reader->AddVariable( "log(Tr_T_P)"            ,  &R_Tr_T_P);           
  reader->AddVariable( "log(Tr_T_MinIPChi2)"    ,  &R_Tr_T_MinIPCHI2);    
  reader->AddVariable( "log(Tr_T_HcalE)"        ,  &R_Tr_T_HcalE);       
  reader->AddVariable( "log(Tr_T_BPVIP)"        ,  &R_Tr_T_BPVIP);       
  reader->AddVariable( "Tr_T_nBDT_ult"     ,  &R_Tr_T_nBDT_ult);    
  reader->AddVariable( "Tr_T_SumBDT_ult"   ,  &R_Tr_T_SumBDT_ult);  
  reader->AddVariable( "Tr_T_Ntr_incone"   ,  &R_Tr_T_Ntr_incone);  
  reader->AddVariable( "Tr_T_Cone_asym_P"  ,  &R_Tr_T_Cone_asym_P);      
  reader->AddVariable( "Tr_T_ConIso_pt_ult",  &R_Tr_T_ConIso_pt_ult);      

  reader->BookMVA( "BDTG", "./weights_pfromLb_MC_Dspi/TMVAClassification_BDTG.weights.xml" );
  
  for(int n = 0; n < mychain->GetEntries() ; ++n) {
    
    if ( n > 500 ) break;
    mychain->GetEntry( n  );
    if (verbose) cout<<" N = " << N <<endl;
    int tagg_cand = 0 ;
    int charge_of_best_track = 0 ;
    float Tr_T_nBDT_ult_max = -1.01 ;
    float Tr_T_P_FROM_Lb_PROB_best = -100;
    int best_index_is = 0;
    // Tagging decision is untagged by default
    B_OSp_DECISION = 0 ;
    
    for (int t_index = 0 ; t_index < N ; t_index++ ) {
      
      //if(Tr_T_MinIPChi2[t_index]<=0 || Tr_T_HcalE[t_index]<=0) continue;
      cout<< " R_Tr_T_HcalE " << log( Tr_T_HcalE[t_index] ) <<endl;
      
      if (verbose) cout<< "track index :" << t_index << endl;
      R_Tr_T_zfirst      = Tr_T_zfirst[t_index];
      R_Tr_T_ETA         = Tr_T_ETA[t_index];
      R_Tr_T_Phi         = Tr_T_Phi[t_index];
      R_Tr_T_PROBNNppi   = Tr_T_PROBNNp[t_index]*(1. - Tr_T_PROBNNpi[t_index]);
      R_Tr_T_PROBNNpk    = Tr_T_PROBNNp[t_index]*(1. - Tr_T_PROBNNk[t_index] );
      R_Tr_T_PROBNNk     = Tr_T_PROBNNp[t_index];
      R_Tr_T_PIDp        = Tr_T_PIDp[t_index];
      R_Tr_T_PIDmu       = Tr_T_PIDmu[t_index];
      R_Tr_T_PIDK        = Tr_T_PIDK[t_index];
      R_Tr_T_P           = log(Tr_T_P[t_index]);
      R_Tr_T_MinIPCHI2   = log(Tr_T_MinIPChi2[t_index]);
      R_Tr_T_HcalE       = log(Tr_T_HcalE[t_index]);
      R_Tr_T_BPVIP       = log(Tr_T_BPVIP[t_index]);
      R_Tr_T_nBDT_ult    = Tr_T_nBDT_ult[t_index];
      R_Tr_T_SumBDT_ult  = Tr_T_SumBDT_ult[t_index];
      R_Tr_T_Ntr_incone  = Tr_T_Ntr_incone[t_index];
      R_Tr_T_Cone_asym_P = Tr_T_Cone_asym_P[t_index];

      float Tr_T_P_FROM_Lb_PROB_index = reader->EvaluateMVA("BDTG" );
      
      if ( Tr_T_P_FROM_Lb_PROB_index > 0.8 && Tr_T_nBDT_ult[t_index] > Tr_T_nBDT_ult_max  ) 
      {
        best_index_is = t_index;
        tagg_cand++;
        Tr_T_nBDT_ult_max = Tr_T_nBDT_ult[t_index] ;
        
        //Tr_T_P_FROM_Lb_PROB_best = Tr_T_P_FROM_Lb_PROB_index  ;
        Tr_T_P_FROM_Lb_PROB = Tr_T_P_FROM_Lb_PROB_index ;
        //cout<<" Tr_T_P_FROM_Lb_PROB_best is now @@  : " <<  Tr_T_P_FROM_Lb_PROB << endl;
        //cout<<"best index is : " << best_index_is << endl;
      }
      
    if( t_index == N - 1 )
    {
      //cout<<"n = " <<N <<endl;
      
      if ( tagg_cand > 0 ) B_OSp_DECISION = Tr_T_Charge[ best_index_is ] ;
      //if ( tagg_cand == 0 ) cout<<"unttagged event "<<endl;
      // cout<<"OS Proton decision : " << B_OSp_DECISION << endl;
      
      for(int ii= 0 ; ii < n_vars ; ii++) {
        TString branch_name_inloop =  ((TBranch*)(*obj_list_br)[ii])->GetName();
        TLeaf *leaf_branch = mychain->GetLeaf( branch_name_inloop  );
        if( leaf_branch->GetLen() <2 ) 
        {
          var_branch_flat[ii] = leaf_branch->GetValue() ;
          if ( branch_name_inloop.Contains("_DEC" ) || branch_name_inloop.Contains("B_ID" ) || branch_name_inloop.Contains("DECISION" ))
          {
            var_branch_flat_int[ii]  = leaf_branch->GetValue() ;
            // cout<<" value of : "<< branch_name_inloop << " is " << var_branch_flat_int[ii] <<endl;
            //cout<<"branch_name_inloop = " << branch_name_inloop <<endl;
          }
        }
        
        else{
          var_branch_flat[ii] = leaf_branch->GetValue( best_index_is ); 
          //cout<<" value of : "<< branch_name_inloop << " is " << var_branch_flat[ii] <<endl;
          
          //var_branch_flat[ii] = leaf_branch->GetValue( t_index ); 
        }
        
      }
      //cout<<"saved best index is : " << best_index_is << endl; 
      //cout<<"saved Tr_T_P_FROM_Lb_PROB is : " << Tr_T_P_FROM_Lb_PROB  << endl; 
      FTT_tree.Fill();       
    }
    
    }
    
  }
  
  
  mutliplicit->Write();
  FTT_tree.Write();                                                                               
  newfile.Close();    
  //mychain->Delete() ; FTT_tree.Delete() ; newfile.Delete() ;
  
  cout<<"======================================================"<<endl;
  cout<<"==++++++++++++++++++++++++++++++++++++++++++++++++++=="<<endl;
  cout<<"======================================================"<<endl;
  return 0;
  
}


//int MakeTrainingTuple()
//{
//  
//  TString Year[] = 
//    { "2011","2012","2015","2016" };
//  TString Channel[] = 
//    { "Bd2JpsiKstar","Bu2JpsiK"   };
//  
//  //for (int i_year = 0 ; i_year <  sizeof(Year)/sizeof(Year[0]) ; i_year ++)
//  // {
//  // for (int i_channel = 0 ; i_channel < sizeof(Channel)/sizeof(Channel[0]) ; i_channel++)
//  for (auto year : Year)
//  {
//    for(auto channel : Channel)
//    {
//      MakeTrainingTuple_fun( year , channel );
//    }
//  }
//  
//  return 0;
//    
//}
//
  
// recyclables :
// Float_t var_branch_original[n_vars];
// std::vector<std::pair<TClassRef,void *> > var_branch_original;
// var_branch_original.resize( n_vars );
// mychain->SetBranchAddress(  Var_br_name , & (var_branch_original[i].second) );
// Inside the loop :
// cout<<" effective number of elements :" << leaf_branch->GetLen()<<endl;
//  cout<<" value of the branch from the original chain "
//  << ((TBranch*)(*obj_list_br)[ii])->GetName() 
//  <<" , "<< leaf_branch->GetValue();
//  
//
//
//Long64_t listEntries  = protontight_list->GetN();
//  Long64_t chainEntries = mychain->GetEntries();
//  cout<<" listEntries : " << listEntries<<endl;
//  
//  // mychain->SetEntryList(newlistptr);
//  mychain->SetEntryList( protontight_list );
//  Int_t treenum=0;

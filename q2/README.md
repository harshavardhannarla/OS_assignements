-> Inputs n,m,o are taken and next n probabilities has been scanned and storred in array.

-> Structs has been created for Pharmaceutical Company, Vaccination Zones,Students. For running 
    them concurrently a thread is intialized in each of the structure.
 
-> A company is runned as follows:
   
   . Index, probability ... are assigned and thread is created  each company waits for a time(2 to 5sec) and
   creates (1 to 5 ) batches of vaccines and capacity as per given instructions.
   
   . This thread then goes to function company status and waits for any zone to take its batches.This is acheived through conditional waiting  whenever a zone takes batch from a company no_of_batches in company is decreased by 1 and a cond_signal is signaled now the comp thread checks if there are any left out batches , if so it again waits in cond loop else goes to prepare batches of vaccines.
   
-> A zone is runned as follows:

   . Index is assigned A thread is created , now this searches for a company which has vaccine batches. if it finds a company then no of batches in the company is reduced ,and probility of that vaccine is copied, no of vaccines is also copied.
   
   . Now this zone  will be going into vaccine phase in which zone creates random no of slots (as per the restrictions given in qun).then goes to a function named zone_status now this zone waits for students to come and get vaccinated this is acheived through cond waiting whenever a student gets vaccine from this zoned batch the rem no of vaccines in the batch is reduced by 1 and a signal is sent to zone , now zone thread checks for rem slots if it is 0 then the zone creates another vacine phase if vacines alloted from company are left over. 

   . this slot creation is done recursively if vaccines are completed then this thread again searches for company.

  . In middle if remaing students becomes zero new phase creation is stopped.

-> A student thread is runned as follows:
   
   . Wait_for_vaccine function is called then this searches for zones with vaccine slots > 0 then slots in zone is decresed and signal is sent to zone for its further steps. now the student gets vacinated and test for antiobodies is done like this:
   
   . a=rand()%100 is taken and compared with probabality of the vacine in that zone (a is less p*100) then +ve else neg.
   
   . After vaccination and testing if he gets -ve for antibodies  and he appeared for less than 3 times he is again sent to vaccination  else left there.

 -> Main function waits for all student threads to complete . 

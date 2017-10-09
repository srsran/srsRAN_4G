EPC Design
==========

Code Structure
**************
.. blockdiag::

The LTE library should include the following functionality that will be common accress multiple nodes.

  blockdiag {
     srsLTE -> lib -> src -> upper -> gtpu.cc;
                             upper -> gtpc.cc; 
                             upper -> s1app.cc;
                             upper -> diameter.cc;
                src -> asn1; 
  }


The examples should aim to include the following classes  

.. blockdiag::

  blockdiag {
     srsLTE -> srsepc -> src -> mme -> mme.cc;
                         src -> hss -> hss.cc;
                         src -> spgw -> spgw.cc;
               srsepc -> examples -> srsepc.cc;
                         examples -> srsmme.cc;
                         examples -> srshss.cc;
                         examples -> srsspgw.cc;
   }


MME Design
**********

The srsMME must maintain three EPC interfaces, the S1-MME, the S11 and the S6a interfaces. The S1-MME will use SCTP (one-to-many or one-to-one) sockets and the S11 will use the GTP-Cv2 protocol over UDP. The S6a will be implmented as a Diameter application over UDP.

The main loop of this 

In a future, multi-threading scenario, 

HSS Design
**********

S-GW Design
***********

P-GW Design
***********

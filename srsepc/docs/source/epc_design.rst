EPC Design
==========

Code Structure
**************

The LTE library should include the following functionality that will be common accress multiple nodes.

.. blockdiag::

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
                                mme -> main.cc;
                         src -> hss -> hss.cc;
                                hss -> main.cc;
                         src -> spgw -> spgw.cc;
                                spgw -> main.cc;
               srsepc -> src -> main.cc;
                         src -> epc.cc;
   }


MME Design
**********

The srsMME must maintain three EPC interfaces, the S1-MME, the S11 and the S6a interfaces. The S1-MME will use an SCTP (many-to-one) socket and the S11 will use the GTP-Cv2 protocol over UDP. The S6a will be implmented as a Diameter application over UDP.

The main loop of the MME will  

HSS Design
**********

S-GW Design
***********

P-GW Design
***********

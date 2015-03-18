filename='../../debug/dist_ra.dat';

enb.NDLRB = 50;
enb.CyclicPrefix = 'Normal';
enb.PHICHDuration = 'Normal';
enb.CFI = 2;
enb.Ng = 'Sixth';
enb.CellRefP = 1;
enb.NCellID = 196;
enb.NSubframe = 5;
enb.NTotalSubframes=1;
enb.DuplexMode = 'FDD';

dci.NDLRB = enb.NDLRB;
dci.DCIFormat = 'SRSLTE_DCI_FORMAT1C';
dci.AllocationType=1;
%dci.Allocation.Bitmap='01111000011110000';
%dci.Allocation.Subset=3;
dci.Allocation.RIV = 33;
dci.Allocation.Gap = 0;
dci.ModCoding=6;
dci.RV=0;
dci.DuplexMode = enb.DuplexMode;
dci.NTxAnts = enb.CellRefP;
pdcch.RNTI = 65535;
pdcch.PDCCHFormat = 3;

pdsch.Modulation='QPSK';
pdsch.RNTI=pdcch.RNTI;
if (enb.CellRefP == 1) 
    pdsch.TxScheme='Port0';
else
    pdsch.TxScheme='TxDiversity';
end
pdsch.NLayers=enb.CellRefP;
pdsch.trblklen=176;
pdsch.RV=dci.RV;

% Begin frame generation
subframe = lteDLResourceGrid(enb);

%%% Create Reference Signals
rsAnt = lteCellRS(enb);
indAnt = lteCellRSIndices(enb);
subframe(indAnt) = rsAnt;

%%% Create PDCCH
[dciMessage,dciMessageBits] = lteDCI(enb,dci);
codedDciBits = lteDCIEncode(pdcch,dciMessageBits);
pdcchInfo = ltePDCCHInfo(enb);
pdcchBits = -1*ones(1,pdcchInfo.MTot);
candidates = ltePDCCHSpace(enb,pdcch,{'bits','1based'});
pdcchBits (candidates(1,1):candidates(1,2)) = codedDciBits;
pdcchSymbols = ltePDCCH(enb, pdcchBits);
pdcchIndices = ltePDCCHIndices(enb,{'1based'});
subframe(pdcchIndices) = pdcchSymbols;

% Create PDSCH
pdsch.prbset = lteDCIResourceAllocation(enb,dci);

[pdschIndices,pdschInfo] = ltePDSCHIndices(enb,pdsch,pdsch.prbset);

dlschTransportBlk=randi([0 1],pdsch.trblklen,1);
pdschcodeword = lteDLSCH(enb,pdsch,pdschInfo.G,dlschTransportBlk);
%crced = lteCRCEncode(dlschTransportBlk, '24A');
%encoded = lteTurboEncode(crced);
%pdschcodeword2 = lteRateMatchTurbo(encoded,pdschInfo.G,pdsch.RV); 
pdschSymbols = ltePDSCH(enb,pdsch,pdschcodeword);

subframe(pdschIndices) = pdschSymbols;

txwaveform = lteOFDMModulate(enb,subframe);

write_complex(filename,sum(txwaveform,2));
fprintf('Written signal to %s\n',filename);



clear
NofENB = 1;

for i=1:NofENB
    enb = lteTestModel('1.1','5MHz');
    enb.TotSubframes = 10;
    if (i == 1)
        tx_signal = lteTestModelTool(enb); 
    else
        tx_signal = tx_signal + lteTestModelTool(enb);
    end
end

corrcfg.PSS='On';
corrcfg.SSS='On';
corrcfg.CellRS='On';

cec = struct;                        % Channel estimation config structure
cec.PilotAverage = 'UserDefined';    % Type of pilot symbol averaging
cec.FreqWindow = 9;                 % Frequency window size
cec.TimeWindow = 9;                 % Time window size
cec.InterpType = 'Linear';            % 2D interpolation type
cec.InterpWindow = 'Centered';       % Interpolation window type
cec.InterpWinSize = 1;               % Interpolation window size


addpath('../../debug/lte/phy/lib/sync/test')
addpath('../../debug/lte/phy/lib/ch_estimation/test')

%tx_signal = signal; 

enb = struct('NDLRB',6,'CyclicPrefix','Normal','DuplexMode','FDD'); 
[cellid, offset] = lteCellSearch(enb, tx_signal,1);

enb.NCellID=cellid;
disp(offset)
enb.NSubframe = 0;

rxWaveform = tx_signal(1+offset:end,:);
rxGrid = lteOFDMDemodulate(enb,rxWaveform);

enb.CellRefP = 4;

[hest, nest] = lteDLChannelEstimate(enb,cec,rxGrid);

griddims = lteResourceGridSize(enb); % Resource grid dimensions
L = griddims(2);  

pbchIndices = ltePBCHIndices(enb);
[pbchRx, pbchHest] = lteExtractResources( ...
    pbchIndices, rxGrid(:,1:L,:), hest(:,1:L,:,:));

% Decode PBCH
[bchBits, pbchSymbols, nfmod4, mib, enb.CellRefP] = ltePBCHDecode( ...
    enb, pbchRx, pbchHest, nest);

% Parse MIB bits
enb = lteMIB(mib, enb)

%plot(angle(hest(:,[1 4],1,1)));

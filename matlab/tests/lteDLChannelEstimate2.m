%lteDLChannelEstimate Downlink channel estimation
%   [HEST NOISEEST] = lteDLChannelEstimate(...) returns HEST, the estimated
%   channel between each transmit and receive antenna and NOISEEST, an
%   estimate of the noise power spectral density on the reference signal
%   subcarriers.
%
%   HEST is an M-by-N-by-NRxAnts-by-CellRefP (optionally
%   M-by-N-by-NRxAnts-by-NLayers for UE-specific beamforming transmission
%   schemes) array where M is the number of subcarriers, N is the number of
%   OFDM symbols, NRxAnts is the number of receive antennas, CellRefP is
%   the number of cell-specific reference signal antenna ports and NLayers
%   is the number of transmission layers. Using the reference signals, an
%   estimate of the power spectral density of the noise present on the
%   estimated channel response coefficients is returned.
%
%   [HEST NOISEEST] = lteDLChannelEstimate(ENB, RXGRID) returns the
%   estimated channel coefficients using the method described in
%   TS36.104/TS36.141 Annex E/F for the purposes of transmitter EVM
%   testing.
%
%   ENB is a structure and must contain the following fields:
%   NDLRB          - Number of downlink resource blocks
%   CellRefP       - Number of cell-specific reference signal antenna ports 
%                    (1,2,4)
%   NCellID        - Physical layer cell identity
%   NSubframe      - Subframe number
%   CyclicPrefix   - Optional. Cyclic prefix length 
%                    ('Normal'(default),'Extended')
%   DuplexMode     - Optional. Duplex mode ('FDD'(default),'TDD')
%   Only required for 'TDD' duplex mode:
%      TDDConfig   - Optional. Uplink/Downlink Configuration (0...6) 
%                    (default 0)
%      SSC         - Optional. Special Subframe Configuration (0...9)
%                    (default 0)
%   Only required for CEC.Reference='CSIRS' below:
%      CSIRefP     - Number of CSI-RS antenna ports (1,2,4,8)
%      CSIRSConfig - CSI-RS configuration index (TS 36.211 Table 
%                    6.10.5.2-1)
%      CSIRSPeriod - Optional. CSI-RS subframe configuration: 
%                    ('On'(default),'Off',Icsi-rs,[Tcsi-rs Dcsi-rs])
%
%   RXGRID is a 3-dimensional M-by-N-by-NRxAnts array of resource elements.
%   The second dimension of RXGRID can contain any whole number of
%   subframes worth of OFDM symbols i.e. for normal cyclic prefix each
%   subframe contains 14 OFDM symbols, therefore N is a multiple of 14.
%   Note: to adhere to the estimation method defined in TS36.104/TS36.141,
%   RXGRID must contain 10 subframes.
%
%   [HEST NOISEEST] = lteDLChannelEstimate(ENB, CEC, RXGRID) returns the
%   estimated channel using the method and parameters defined by the
%   configuration structure CEC.
%
%   CEC is a structure which defines the type of channel estimation
%   performed. CEC must contain a set of the following fields:
%   PilotAverage  - Type of pilot averaging ('TestEVM', 'UserDefined')
%   FreqWindow    - Size of window in resource elements used to average
%                   over frequency.
%   TimeWindow    - Size of window in resource elements used to average
%                   over time 
%   InterpType    - Type of 2D interpolation used(see <a href="matlab:
%                   doc('griddata')">griddata</a> for types)
%   InterpWindow  - Interpolation window type: ('Causal','Non-causal',
%                   'Centred','Centered'). Note: 'Centred' and 'Centered'
%                   are equivalent
%   InterpWinSize - Interpolation window size:
%                   'Causal','Non-causal' - any number >=1
%                   'Centred','Centered'  - odd numbers >=1
%
%   The 'TestEVM' pilot averaging will ignore other structure fields in
%   CEC, and the method follows that described in TS36.104/TS36.141 Annex
%   E/F for the purposes of transmitter EVM testing.
%
%   The 'UserDefined' pilot averaging uses a rectangular kernel of size
%   CEC.FreqWindow-by-CEC.TimeWindow and performs a 2D filtering operation
%   upon the pilots. Note that pilots near the edge of the resource grid
%   will be averaged less as they have no neighbors outside of the grid,
%   or a limited number of neighbors outside of the grid obtained by the
%   creation of virtual pilots.
%   
%   [HEST NOISEEST] = lteDLChannelEstimate(ENB, CHS, CEC, RXGRID) returns
%   the estimated channel given cell-wide settings structure ENB, PDSCH
%   transmission configuration CHS and channel estimator configuration CEC.
%   CHS must include the following fields:
%   TxScheme - Transmission scheme, one of:
%              'Port0'       - Single-antenna port, Port 0
%              'TxDiversity' - Transmit diversity scheme
%              'CDD'         - Large delay CDD scheme
%              'SpatialMux'  - Closed-loop spatial multiplexing scheme
%              'MultiUser'   - Multi-user MIMO scheme
%              'Port5'       - Single-antenna port, Port 5
%              'Port7-8'     - Single-antenna port, port 7 (when
%                              NLayers=1); Dual layer transmission, port 7 
%                              and 8 (when NLayers=2)
%              'Port8'       - Single-antenna port, Port 8
%              'Port7-14'    - Up to 8 layer transmission, ports 7-14
%   PRBSet   - A 1- or 2-column matrix, containing the 0-based Physical 
%              Resource Block indices (PRBs) corresponding to the resource 
%              allocations for this PDSCH.
%   RNTI     - Radio Network Temporary Identifier (16-bit)
%
%   For the 'Port5', 'Port7-8', 'Port8' and 'Port7-14' transmission
%   schemes, the channel estimator configuration structure CEC contains the
%   following additional field:
%   Reference - Optional. Specifies point of reference (signals to 
%               internally generate) for channel estimation. 
%               ('DMRS'(default), 'CSIRS')
%
%   For the 'Port5', 'Port7-8', 'Port8' and 'Port7-14' transmission
%   schemes, with Reference='DMRS', the channel estimation is performed
%   using UE-specific reference signals and the returned channel estimate
%   will be of size M-by-N-by-NRxAnts-by-NLayers. Alternatively, with
%   Reference='CSIRS' the channel estimation is performed using the CSI
%   reference signals (CSI) and the returned channel estimate will be of
%   size M-by-N-by-NRxAnts-by-CSIRefP. For other transmission schemes the
%   channel estimation is performed using cell-specific reference signals
%   and the channel estimate will be of size M-by-N-by-NRxAnts-by-CellRefP.
%   Note that CSI-RS based channel estimation and hence Reference='CSIRS'
%   is strictly only valid within the standard for the 'Port7-14'
%   transmission scheme. The optional CSIRSPeriod parameter controls the
%   downlink subframes in which CSI-RS will be present, either always 'On'
%   or 'Off', or defined by the scalar subframe configuration index Icsi-rs
%   (0...154) or the explicit subframe periodicity and offset pair [Tcsi-rs
%   Dcsi-rs] (TS 36.211 Section 6.10.5.3).
%
%   For the 'Port7-8' and 'Port7-14' transmission schemes with
%   'UserDefined' pilot averaging, if CEC.TimeWindow = 2 or 4 and
%   CEC.FreqWindow=1 the estimator will enter a special case where an
%   averaging window of 2 or 4 pilots in time will be used to average the
%   pilot estimates; the averaging is always applied across 2 or 4 pilots,
%   regardless of their separation in OFDM symbols. This operation ensures
%   that averaging is always done on 2 or 4 pilots. This provides the
%   appropriate "despreading" operation required for the case of UE RS
%   ports / CSI-RS ports which occupy the same time/frequency locations but
%   use different orthogonal covers to allow them to be differentiated at
%   the receiver. For the CSI-RS and any number of configured CSI-RS ports
%   (given by ENB.CSIRefP), the pilot REs occur in pairs, one pair per
%   subframe, that require averaging with CEC.TimeWindow=2 and will result
%   in a single estimate per subframe. For the UE RS with between 1 and 4
%   layers (given by CHS.NLayers), the pilot REs occur in pairs, repeated
%   in each slot, that require averaging with CEC.TimeWindow=2 and will
%   result in two estimates per subframe, one for each slot; for between 5
%   and 8 layers, the pairs are distinct between the slots of the subframe
%   and the required averaging is CEC.TimeWindow=4, resulting in one
%   estimate per subframe.
%
%   Example: 
%   Transmit RMC R.12 (4-antenna transmit diversity), model the propagation 
%   channel by combining all transmit antennas onto one receive antenna, 
%   OFDM demodulate and finally channel estimate.
%
%   enb = lteRMCDL('R.12');
%   cec = struct('FreqWindow',1,'TimeWindow',1,'InterpType','cubic',...
%             'PilotAverage','UserDefined','InterpWinSize',3,...
%             'InterpWindow','Causal');
%   txWaveform = lteRMCDLTool(enb,[1;0;0;1]);
%   rxWaveform = sum(txWaveform,2);
%   rxGrid = lteOFDMDemodulate(enb,rxWaveform);
%   hest = lteDLChannelEstimate(enb,cec,rxGrid);
%   
%   See also lteOFDMDemodulate, lteEqualizeMMSE, lteEqualizeZF, 
%   lteDLPerfectChannelEstimate, griddata.

%   Copyright 2009-2013 The MathWorks, Inc.

function [H_EST, NoisePowerEst, AvgEstimates, Estimates] = lteDLChannelEstimate2(varargin)

    if(isstruct(varargin{2}))
        if(isstruct(varargin{3}))
            PDSCH = varargin{2};
            CEC = varargin{3};
            RXGRID = varargin{4};            
        else
            PDSCH = [];
            CEC = varargin{2};
            RXGRID = varargin{3};
        end
    else
        % If no configuration structure then use the TestEVM method
        PDSCH = [];
        CEC.PilotAverage = 'TestEVM';
        RXGRID = varargin{2};        
    end          

    ENB = varargin{1};
    % Get dimensions of resource grid
    Dims = lteDLResourceGridSize(ENB);
    K = Dims(1);
    Lsf = Dims(2);
    
    % Determine number of subframes
    nsfs = size(RXGRID,2)/Dims(2);
    
    % Determine number of Tx- and RxAntennas
    NRxAnts = size(RXGRID,3);    
    if (~isempty(PDSCH) && any(strcmpi(PDSCH.TxScheme,{'Port5' 'Port7-8' 'Port8' 'Port7-14'})))
        if (~isfield(CEC,'Reference'))
            CEC.Reference='DMRS';
        end
        if (strcmpi(CEC.Reference,'DMRS')==1)
            NTx = PDSCH.NLayers;
        elseif (strcmpi(CEC.Reference,'CSIRS')==1)
            NTx = ENB.CSIRefP;
        else
            error('lte:error','Reference must be "DMRS" or "CSIRS", see help for details.');
        end            
    else
        if (~isfield(CEC,'Reference'))
            CEC.Reference='CellRS';
        end
        NTx = ENB.CellRefP;
    end
    
    % Initialize size of estimated channel grid
    H_EST = zeros([size(RXGRID,1) size(RXGRID,2) size(RXGRID,3) NTx]);
    
    % Preallocate noise power estimate vector for speed
    noiseVec = zeros(size(NRxAnts,NTx));
    
    if (NTx == 4)
        nn=3;
    else
        nn=NTx;
    end
    RefEstimates=zeros(nn,2*2*2*ENB.NDLRB*10);
        
    for rxANT = 1:NRxAnts
        for nTxAntLayer = 1:NTx
                
            % Extract pilot symbols from received grid and calculate least 
            % squares estimate
            [ls_estimates,specialvec,DwPTS]= GetPilotEstimates(ENB,nTxAntLayer-1,RXGRID(:,:,rxANT),PDSCH,CEC);
                        
            Estimates(nTxAntLayer,1:length(ls_estimates(3,:))) = ls_estimates(3,:);
            
            % Average the pilots as defined by CEC.PilotAverage, this can
            % be 'UserDefined' or 'TestEVM'. 
            % Note: Setting the window size to 1x1 is equivalent to no 
            % averaging. It is recommended that no averaging should be
            % carried out when a high SNR is present, as this would have an
            % adverse effect on the least squares estimates
            if strcmpi(CEC.PilotAverage,'UserDefined')&&(CEC.FreqWindow==1)&&(CEC.TimeWindow==1)
                P_EST = ls_estimates;
                ScalingVec = 0;
            else
                [P_EST, ScalingVec]= PilotAverage(PDSCH,CEC,H_EST,ls_estimates);
            end

            AvgEstimates(nTxAntLayer,1:length(P_EST(3,:))) = P_EST(3,:);

            if strcmpi(CEC.PilotAverage,'TestEVM')
                % Channel estimation as defined in TS36.141 Annex F.3.4 is
                % performed, the pilots are averaged in time and frequency
                % and the resulting vector is linearly interpolated.
                %----------------------
                % Interpolate eq coefficients extrapolating to account for
                % missing pilots at the edges
                interpEqCoeff = interp1(find(P_EST~=0),P_EST(P_EST~=0),(1:length(P_EST)).','linear','extrap');
                % ---------------------
                % The DC carrier needs to be accounted for during
                % interpolation
                % Get pilot symbol index values
                ind = lteCellRSIndices(ENB,0);
                % Using initial index value calculate position of pilot
                % symbols within interpolated coefficients
                if ind(1)-3>0
                    vec = (ind(1)-3):3:K;
                else
                    vec = ind(1):3:K;
                end
                % Determine the index values of the pilots either side of
                % the DC carrier
                preDC = vec(length(vec)/2);
                postDC = vec(1+length(vec)/2);
                % Interpolate the section using the DC carrier and insert
                % this into the interpolated vector containing the
                % coefficients
                interpEqCoeffTemp = interp1(1:4:5,[interpEqCoeff(preDC) interpEqCoeff(postDC)],1:5,'linear');
                interpEqCoeff(preDC:K/2) = interpEqCoeffTemp(1:length(interpEqCoeff(preDC:K/2)));
                interpEqCoeff(1+K/2:postDC) = interpEqCoeffTemp(2+length(interpEqCoeff(preDC:K/2)):end);
                %----------------------
                % Generate grid by replicating estimated equalizer channel
                % coefficients
                H_EST(:,:,rxANT,nTxAntLayer) = repmat(interpEqCoeff,1,Lsf*nsfs);
                % The value of the noise averaged pilot symbols are placed
                % into P_EST(3,:) matrix and these are used to determine
                % the noise power present on the pilot symbol estimates of
                % the channel.
                P_EST = ls_estimates;
                tempGrid = squeeze(H_EST(:,:,rxANT,nTxAntLayer));
                P_EST(3,:) = tempGrid(sub2ind(size(H_EST),P_EST(1,:),P_EST(2,:)));

            else
                % Channel estimation is performed by analyzing up to
                % 'CEC.InterpWinSize' subframes together and estimating 
                % virtual pilots outwith the bounds of the subframe.
                % Averaging window can be set to Causal, Non-causal or
                % Centred, Centered.              
                
                % Using the desired subframeAveraging type the initial
                % position of the window is determined
                if(CEC.InterpWinSize>=1)  
                    if strcmpi(CEC.InterpWindow,'Centred')||strcmpi(CEC.InterpWindow,'Centered')
                        % For Centered averaging the window size must be odd
                        if ~mod(CEC.InterpWinSize,2)
                            error('lte:error','Window size must be odd for centered window type');
                        end
                        % For Centered windowing both current,future and past
                        % data are used to estimate current channel coefficients
                        x = floor(CEC.InterpWinSize/2);
                        y = floor(CEC.InterpWinSize/2);
                    elseif strcmpi(CEC.InterpWindow,'Causal')
                        % For causal windowing current and past data are used
                        % to estimate current subframe channel coefficients
                        x = 0;
                        y = CEC.InterpWinSize-1;
                    elseif strcmpi(CEC.InterpWindow,'Non-causal')
                        % For non-causal windowing current and future data are
                        % used to estimate current subframe channel
                        % coefficients
                        x = CEC.InterpWinSize-1;
                        y = 0;
                    else
                        error('lte:error','Channel estimation structure field InterpWindow must be one of the following: Causal,Non-causal,Centred,Centered');
                    end
                else
                    error('lte:error','InterpWinSize cannot be less than 1');
                end
                
                % Interpolate using averaged pilot estimates and defined
                % interpolation settings
                for sf = 0:nsfs-1
                    if specialvec(sf+1)=='U'                        
                        H_EST(:,sf*Lsf+1:(sf+1)*Lsf,rxANT,nTxAntLayer) = NaN;
                    else
                        % Extract the pilots from required subframes                 
                        p_use = P_EST;
                        p_use(:,p_use(4,:)>(sf+1)+x)=[];
                        p_use(:,p_use(4,:)<(sf+1)-y)=[];
                        
                        % Account for DC offset
                        p_use(1,(p_use(1,:)>K/2)) = p_use(1,(p_use(1,:)>K/2))+1;

                        if strcmpi(CEC.InterpType,'Cubic')||strcmpi(CEC.InterpType,'Linear')
                            % VPVEC is used to determine if virtual pilots are
                            % needed at the beginning or end of the
                            % interpolation window, if the current subframe,sf,
                            % is located at beginning or end of the window then
                            % virtual pilots are created accordingly
                            vpvec = unique(p_use(4,:));
                            
                            if (strcmp(CEC.Reference,'CellRS')==1)
                                % Create virtual pilots and append to vector
                                % containing pilots using cell-specific RS
                                % methodology.
                                vps = createVirtualPilots(ENB,p_use(1:3,:),1,1,(sf+1==min(vpvec)),(sf+1==max(vpvec)));
                                p_use = [p_use(1:3,:) vps];
                            else
                                % Create edge virtual pilots suitable for 
                                % UE RS which can have partial bandwidth,
                                % or CSI-RS. 
                                if (~isempty(p_use))
                                    vps = createEdgeVirtualPilots(ENB,p_use(1:3,:));
                                    p_use = [p_use(1:3,:) vps];
                                end
                            end
                        end
                        % Perform 2D interpolation                       
                        % Interpolation is carried out on a (K+1)-by-L
                        % matrix to account for DC offset being added in
                        if (~isempty(p_use))
                            Htemp = griddata(p_use(2,:)-Lsf*sf,p_use(1,:),p_use(3,:),1:Lsf,(1:K+1)',CEC.InterpType); %#ok<GRIDD>
                            % Remove DC offset
                            Htemp(1+(K/2),:) = [];

                            if specialvec(sf+1)=='S'
                                Htemp(:,DwPTS+1:Lsf) = NaN;
                            end
                            H_EST(:,sf*Lsf+1:(sf+1)*Lsf,rxANT,nTxAntLayer) = Htemp;

                            if isnan(H_EST)
                                error('lte:error','H_EST NaN');
                            end
                        end
                    end
                end  
            end
            if nTxAntLayer<3 || (~isempty(PDSCH) && any(strcmpi(PDSCH.TxScheme,{'Port7-8' 'Port7-14'})))
                % The noise level present can be determined using the noisy
                % least squares estimates of the channel at pilot symbol
                % locations and the noise averaged pilot symbol estimates of the
                % channel
                noise = ls_estimates(3,~isnan(ls_estimates(3,:))) - P_EST(3,~isnan(P_EST(3,:)));
                if strcmpi(CEC.PilotAverage,'UserDefined')
                    noise = sqrt(ScalingVec./(ScalingVec+1)).*noise;
                end
                                
                % Additional averaging for noise estimation in LTE-A case,
                % to suppress interference from orthogonal sequences on
                % other antennas in same time-frequency locations.
                if (~isempty(PDSCH) && any(strcmpi(PDSCH.TxScheme,{'Port7-8' 'Port7-14'})) &&...
                    (CEC.TimeWindow==2 || CEC.TimeWindow==4) && CEC.FreqWindow==1)                                    
                    if (~isempty(ls_estimates)) 
                        if (strcmpi(CEC.Reference,'DMRS')==1)
                            temp=[];
                            pilotSC = unique(ls_estimates(1,:));    
                            for i = pilotSC
                                x=find(ls_estimates(1,:)==i); 
                                x=[x(1) x(end)];
                                temp=[temp mean(noise(x))*2]; %#ok<AGROW>
                            end                                                      
                            noise=temp;
                        end
                    end                                    
                end

                % Taking the variance of the noise present on the pilot symbols
                % results in a value of the noise power for each transmit and 
                % receive antenna pair
                if (isempty(noise))
                    noiseVec(rxANT,nTxAntLayer)=NaN;
                else
                    noiseVec(rxANT,nTxAntLayer) = mean(noise.*conj(noise));
                end
            end
        end
    end
    
    % The mean of the noise power across all the transmit/receive antenna
    % pairs is used as the estimate of the noise power
    NoisePowerEst = mean(mean(noiseVec));
        
end

% GetPilotEstimates Obtain the least squares estimates of the reference
%                  signals
%   [ls_estimates] = GetPilotEstimates(ENB,nTxAntLayer,RXGRID) Extracts the
%   reference signals for a specific transmit/receive antenna pair and
%   calculates their least squares estimate. The results are placed in a  
%   3xNp matrix containing the subcarrier and OFDM symbol location, row and 
%   column subscripts, and value. Np is the number of cell specific 
%   reference (pilot) symbols per resource grid
%   
%   RXGRID is an MxN array where M is the number of subcarriers,
%   N is the number of OFDM symbols. Dimension M must be 
%   12*NDLRB where NDLRB must be {6,15,25,50,75,100}. 
%   Dimension N must be a multiple of number of symbols in a subframe L, 
%   where L=14 for normal cyclic prefix and L=12 for extended cyclic
%   prefix. 
%
%   nTxAntLayer defines which transmit antennas' or layers' pilot symbols to extract
% 
%   ENB is a structure and must contain the following fields:
%   NDLRB           - Number of downlink resource blocks
%   NCellID         - Physical layer cell identity
%   CellRefP        - Number of transmit antenna ports {1,2,4}
%   CyclicPrefix    - Optional. Cyclic prefix length{'Normal'(default),'Extended'}
%
% 
%   Example
%   Return the least squares estimate of the pilots symbols in a 3xNp array
%   where the first row is the subcarrier index,k the second row is the OFDM
%   symbol,l and the third row defines the least squares estimate of the
%   pilot located at that position.
%
%   enb=struct('NDLRB',6,'CellRefP',1,'NCellID',0,'CyclicPrefix','Normal');
%   rxGrid=ones(lteDLResourceGridSize(enb));
%   nTxAntLayer=0;
%   ls_estimates = GetPilotEstimates(enb,nTxAntLayer,rxGrid).'
%   ans =
%         1.0000             1.0000            -0.7071 - 0.7071i
%         7.0000             1.0000            -0.7071 - 0.7071i
%         13.0000            1.0000            -0.7071 - 0.7071i
%           .                 .                      .
%           .                 .                      .
%           .                 .                      .
%         58.0000           12.0000            -0.7071 + 0.7071i
%         64.0000           12.0000             0.7071 - 0.7071i
%         70.0000           12.0000             0.7071 - 0.7071i

%   Copyright 2009-2010 The MathWorks, Inc.
function [ls_estimates,specialvec,DwPTS] = GetPilotEstimates(ENB,nTxAntLayer,RXGRID,PDSCH,CEC)  

% Get dimensions of resource grid
Dims = lteDLResourceGridSize(ENB);
nsfs = size(RXGRID,2)/Dims(2);
K = Dims(1);
L = Dims(2);

if (rem(nsfs,1)~=0)
    error('lte:error','The received grid input must contain a whole number of subframes.');
end

if (~isempty(PDSCH) && any(strcmpi(PDSCH.TxScheme,{'Port5' 'Port7-8' 'Port8' 'Port7-14'})))
    if (strcmp(CEC.Reference,'DMRS')==1)
        % create UE RS indices
        PDSCH.NTxAnts=0;
        PDSCH.W=[];
        linearIndSF = lteDMRSIndices(ENB,PDSCH,'mat');
    else
        % create CSI-RS indices and symbols and deal with zero removal
        linearIndSF = lteCSIRSIndices(ENB,'mat');
        CsiRS = lteCSIRS(ENB,'mat');
        CsiRS = CsiRS(:,nTxAntLayer+1);
        linearIndSF(CsiRS==0,:)=[];
        CsiRS(CsiRS==0)=[];
    end
    % If more than one TxAntenna is used we need to adjust indices values so that
    % they start in first antenna plane    
    linearIndSF = linearIndSF(:,nTxAntLayer+1) - (K*L*nTxAntLayer);
else
    % Extract linear indices of reference signals for particular TxAntenna
    dumENB = ENB;
    dumENB.NSubframe = 0;
    linearIndSF = lteCellRSIndices(dumENB,nTxAntLayer);
    % If more than one TxAntenna is used we need to adjust indices values so that
    % they start in first antenna plane
    linearIndSF = linearIndSF - (K*L*nTxAntLayer);
end
    
% Create grid for pilot symbols
linearIndGrid = zeros(size(linearIndSF,1),nsfs);
idealPilotGrid = zeros(size(linearIndSF,1),nsfs);
offset = double(ENB.NSubframe);
specialvec = char(zeros(1,0));
DwPTS = 0;
for sf = ENB.NSubframe:ENB.NSubframe+nsfs-1
    ENB.NSubframe = mod(sf,10);
    linearIndGrid(:,sf-offset+1) = (linearIndSF+(sf*K*L));
    if (~isempty(PDSCH) && any(strcmpi(PDSCH.TxScheme,{'Port5' 'Port7-8' 'Port8' 'Port7-14'}))) 
        if (strcmp(CEC.Reference,'DMRS')==1)
            % create UE RS symbols
            DMRS = lteDMRS(ENB,PDSCH,'mat');
            idealPilotGrid(1:length(DMRS),sf-offset+1) = DMRS(:,nTxAntLayer+1);        
        else            
            % use CSI-RS symbols created earlier
            idealPilotGrid(1:length(CsiRS),sf-offset+1) = CsiRS;        
        end
    else
        CellRS = lteCellRS(ENB, nTxAntLayer);
        idealPilotGrid(1:length(CellRS),sf-offset+1) = CellRS;
    end
    dupinfo = lteDuplexingInfo(ENB);
    if strcmpi(dupinfo.SubframeType,'Special')
        specialvec = [specialvec 'S']; %#ok<AGROW>
        DwPTS = dupinfo.NSymbolsDL;
    elseif strcmpi(dupinfo.SubframeType,'Uplink')
        specialvec = [specialvec 'U']; %#ok<AGROW>
    elseif strcmpi(dupinfo.SubframeType,'Downlink')
        specialvec = [specialvec  'D']; %#ok<AGROW>
    else 
        error('lte:error','Invalid SubframeType, in structure dupinfo');        
    end
end

if (~isempty(idealPilotGrid))
    ulvec = find(idealPilotGrid(size(idealPilotGrid,1),:)==0);
    linearIndGrid(:,ulvec) = [];
    idealPilotGrid(:,ulvec) = [];
else
    ulvec = [];
end

linearIndGrid = linearIndGrid - (offset*K*L);
% Extract the row and column subscripts of the pilot symbols for entire
% grid
[p_estSC, p_estSym] = ind2sub(size(RXGRID),linearIndGrid);

% Calculate least squares channel estimates at pilot locations
p_est = RXGRID(linearIndGrid)./idealPilotGrid;

% Create a vector - [k;l;p_est]
sfref = repmat((1:nsfs),length(linearIndSF),1);
sfref(:,ulvec) = [];
ls_estimates = [double(p_estSC(:).') ; double(p_estSym(:).') ; p_est(:).';double(sfref(:)).'];

end

%PilotAverage Average reference signals
%   [P_EST] = PilotAverage(PDSCH,CEC,H_EST,LS_EST) performs a moving average of pilot
%   symbols
%
%   LS_EST is a 3xNp matrix containing the least square estimates of the 
%   pilots symbols and their column and row indices within the received
%   grid. 
%   LS_EST = [k;l;p_est]
%   H_EST is an MxN matrix and defines the size of the grid that the
%   averaging will be performed on
%
%   CEC is a structure which defines the type of channel estimation
%   performed. CEC must contain a set of the following fields:
%   PilotAverage    -   Type of pilot averaging {'TestEVM', 'UserDefined'}
%   FreqWindow      -   Size of window used to average in frequency in
%                       resource elements.
%   TimeWindow      -   Size of window used to average in time in resource
%                       elements
%   InterpType      -   Type of 2D interpolation used(see griddata for types)
% 
%   The dimensions of the averaging window are defined in structure CEC. 
%   The window is defined in terms of Resource Elements, and depending on
%   the size of the averaging window, averaging will be performed in either
%   the time or frequency direction only, or a combination of both creating
%   a square/rectangular window. The pilot to be averaged will always be
%   placed at the center of the window, therefore the window size must be 
%   an odd number.
%
%   Frequency Direction: 9x1               Time Direction:  1x9
%
%         x
%         x                     
%         x                     
%         x                     
%         P                                 x x x x P x x x x
%         x                     
%         x                     
%         x
%         x
%   
%
% Square window:  9x9
%                      x x x x x x x x x
%                      P x x x x x x P x
%                      x x x x x x x x x
%                      x x x x x x x x x                  
%                      x x x x P x x x x                   
%                      x x x x x x x x x                  
%                      x x x x x x x x x                   
%                      P x x x x x x P x                    
%                      x x x x x x x x x
% 
%   Performing EVM compliance testing as per TS36.141 AnnexF.3.4, requires
%   time averaging be done over 10 subframes(i.e. 1 Frame) across each
%   pilot symbol carrying subcarrier creating a TotalNumberPilotsx1 vector.
%   This is then frequency averaged using a moving window average with a window
%   size of 19. This type of averaging can be performed by setting the
%   PilotAverage type in structure CEC to 'TestEVM'.

%   Copyright 2009-2013 The MathWorks, Inc.

function [P_EST, scalingVec] = PilotAverage(PDSCH,CEC,H_EST,P_EST)

    switch lower(CEC.PilotAverage)

        case 'userdefined'
            if(CEC.FreqWindow<1)||(CEC.TimeWindow<1)
                error('lte:error','Frequency and time averaging window size cannot be less than 1');
            end

            if (~isempty(PDSCH) && any(strcmpi(PDSCH.TxScheme,{'Port7-8' 'Port7-14'})) &&...
                    (CEC.TimeWindow==2 || CEC.TimeWindow==4) && CEC.FreqWindow==1)

                % Perform averaging in time direction using a moving window 
                % of size N = CEC.TimeWindow
                N = CEC.TimeWindow;

                if (~isempty(P_EST))
                    % Average only subcarriers which contain RS symbols. Extract RS
                    % symbols on a per subcarrier basis and use window to average.
                    for subcarrier = unique(P_EST(1,:))                        

                        % Store DRS symbols from relevant slot in vector for easy access
                        symbVec = (P_EST(3,P_EST(1,:)==subcarrier)).';

                         % Define temporary vector to store the averaged values
                        avgVec = zeros(size(symbVec));

                        % Check length of input at least equal to size of window;
                        % if not, reduce window size. Despreading will be incomplete
                        % here 
                        if (length(symbVec) < N)                       
                            N = length(symbVec);
                        end               

                        % Determines position of window w.r.t element being averaged,
                        % and performs the averaging
                        for symbNo = 1:length(symbVec)
                            if (symbNo-(N/2+1)<=0)
                                avgVec(symbNo)= sum(symbVec(1:N))/N;
                            else
                                avgVec(symbNo) = sum(symbVec(end-(N-1):end))/N;
                            end
                        end

                        % Update P_EST with averaged values
                        P_EST(3,P_EST(1,:)==subcarrier)= avgVec;
                    end
                    
                end

                % This vector is used to scale the noise by the number of averaging
                % elements in the window
                scalingVec = ones(size(P_EST(3,:)))*N; 

            else

                if (strcmpi(CEC.InterpWindow,'Centred')||strcmpi(CEC.InterpWindow,'Centered'))
                    if (~mod(CEC.FreqWindow,2)||~mod(CEC.TimeWindow,2))
                        error('lte:error','Window size must be odd in time and frequency for centred/centered window type');
                    end
                end             

                % Define number of subcarriers
                K = size(H_EST,1);

                % Define an empty resource grid with the DC offset subcarrier
                % inserted  
                grid = zeros(size(H_EST,1)+1,size(H_EST,2));

                % Account for DC offset
                P_EST(1,(P_EST(1,:)>K/2)) = P_EST(1,(P_EST(1,:)>K/2))+1;

                % Place the pilot symbols back into the received grid with the
                % DC offset subcarrier in place
                grid(sub2ind(size(grid),P_EST(1,:),P_EST(2,:))) = P_EST(3,:); 

                % Define convolution window
                kernel = ones(CEC.FreqWindow,CEC.TimeWindow);

                % Perform convolution
                grid2=grid;
                grid = conv2(grid,kernel,'same');         

                % Extract only pilot symbol location values and set the rest of
                % the grid to zero
                tempGrid = zeros(size(grid));
                tempGrid(sub2ind(size(grid),P_EST(1,:),P_EST(2,:))) = grid(sub2ind(size(grid),P_EST(1,:),P_EST(2,:)));
                grid = tempGrid;

                % Normalize pilot symbol values after convolution
                [grid, scalingVec] = normalisePilotAverage(CEC,P_EST,grid);           

                % Place averaged values back into pilot symbol matrix
                P_EST(3,:) = grid(sub2ind(size(tempGrid),P_EST(1,:),P_EST(2,:)));

                % Remove DC offset
                P_EST(1,(P_EST(1,:)>K/2)) = P_EST(1,(P_EST(1,:)>K/2))-1;

            end


        case 'testevm'
            % As defined in TS36.141 Annex F.3.4 pilots are averaged in
            % time across all pilot carrying subcarriers. The resulting
            % vector is averaged in frequency direction with a moving
            % averaging window of 19.

            % Get phase (theta) and magnitude(radius) of complex estimates
            [theta,radius] = cart2pol(real(P_EST(3,:)),imag(P_EST(3,:)));

            % Declare vector to temporarily store averaged phase and 
            % magnitude
            phasemagVec = zeros(size(H_EST,1),2);

            % Perform averaging in time and frequency for phase and
            % magnitude separately and recombine at end
            for phaseOrMag = 1:2 
                % Define temporary P_EST vector and set estimates row to
                % phase then magnitude
                P_EST_temp = P_EST;
                if phaseOrMag==1
                    P_EST_temp(3,:) = theta;
                elseif phaseOrMag==2
                    P_EST_temp (3,:) = radius;
                end

                % Declare averaging vector
                avgVec = zeros(size(H_EST,1),1);
                vec = [];

                % Average across pilot symbol carrying subcarriers in time,
                % unwrapping performed on a subcarrier basis
                for i = 1:size(H_EST,1)
                    if ~isempty(P_EST(3,(P_EST(1,:)==i)))
                        if phaseOrMag==1
                            avgVec(i) = mean(unwrap(P_EST_temp(3,(P_EST(1,:)==i))));

                        elseif phaseOrMag==2
                            avgVec(i) = mean(P_EST_temp(3,(P_EST(1,:)==i)));
                        end
                        vec = [vec i]; %#ok<AGROW>
                    end
                end

                % Remove subcarriers which contained no pilot symbols, then
                % perform frequency averaging
                avgVec = avgVec(vec);

                % Perform averaging in frequency direction using a moving
                % window of size N, N = 19 in TS36.141 Annex F.3.4
                % Performs a moving average of window size N. At the edges, where less than
                % N samples are span the window size is reduce to span 1, 3, 5, 7 ...
                % samples.
                N = 19;

                % avgVec must be a column vector
                if (size(avgVec,2) > 1)
                    error('lte:error','Input to moving average function must be a column vector.')
                end

                % N max window size 19 as defined in Annex F.3.4
                if ~mod(N,2)
                    error('lte:error','Window size N must be odd');
                end

                if (length(avgVec) < N)
                    %    sprintf ('Input signal must have at least %d elements',N);
                    error('lte:error','Input signal must have at least %d elements',N);
                end

                % Use filter to perform part of the averaging (not normalized)
                zeroPad = zeros(N-1,1);
                data = [avgVec; zeroPad];
                weights = ones(N,1);
                freqAvg = filter(weights,1,data);

                % Remove unwanted elements
                removeIdx = [  2:2:N ( length(data)-(2:2:N)+1 )];
                freqAvg(removeIdx) = [];

                % Normalization factor
                normFactor = [1:2:N-2 N*ones(1,length(freqAvg)-(N-1)) N-2:-2:1]';
                freqAvg = freqAvg./normFactor;

                % Place frequency averaged pilots into temporary storage
                % vector
                phasemagVec(vec,phaseOrMag) = freqAvg;

            end
            % Convert averaged symbols back to Cartesian coordinates
            [X,Y] = pol2cart(phasemagVec(:,1),phasemagVec(:,2));
            P_EST = complex(X,Y);
            scalingVec = [];

        otherwise
            error('lte:error','Channel estimation structure field PilotAverage must be one of the following: "UserDefined" or "TestEVM"'); 

    end 
end

function [avgGrid, scalingVec] = normalisePilotAverage(CEC,p_est,grid)

            % Determine total number of pilots within half a subframe
            nPilots = length(p_est);
            
            avgGrid = zeros(size(grid));
            scalingVec = zeros(1,size(p_est,2));
            
            for n = 1:nPilots
                % Determine in which subcarrier and OFDM symbol pilot is located 
                sc = p_est(1,n);
                sym = p_est(2,n);
                % Determine number of REs to look at either side of pilot 
                % symbol in time and frequency
                half_freq_window = floor(CEC.FreqWindow/2);
                half_time_window = floor(CEC.TimeWindow/2);
                % Set the location of the window at the back of the pilot 
                % to be averaged in frequency direction
                upperSC = sc-half_freq_window;
                % If this location is outwith the grid dimensions set it to
                % lowest subcarrier value
                if upperSC<1
                    upperSC = 1;
                end
                % Set the location of the window in front of the pilot to 
                % be averaged in frequency direction
                lowerSC =  sc+half_freq_window;
                % If this location is outwith the grid dimensions set it to
                % highest subcarrier value
                if lowerSC>size(grid,1)
                    lowerSC = size(grid,1);
                end
                % Set the location of the window in front of the pilot to 
                % be averaged in time direction
                leftSYM = sym-half_time_window;
                % If this location is outwith the grid dimensions set it to
                % lowest OFDM symbol value
                if leftSYM<1
                    leftSYM = 1;
                end
                % Set the location of the window at the back of the pilot 
                % to be averaged in time direction
                rightSYM = sym+half_time_window;
                % If this location is outwith the grid dimensions set it to
                % highest OFDM symbol value
                if rightSYM>size(grid,2)
                    rightSYM = size(grid,2);
                end
                
                % Define the window to average using the determined 
                % subcarrier and OFDM symbol values
                avgVec = grid(upperSC:lowerSC,leftSYM:rightSYM);
                
                % Remove the zero values from the window so that the average is
                % calculated using only valid pilots
                avgVec = avgVec(avgVec~=0);
                % Average the desired pilot using all pilots within 
                % averaging window
                if isempty(avgVec)
                    avgVec = 0;
                end

                avgGrid(sc,sym) = grid(sc,sym)/numel(avgVec);
                scalingVec(n) = numel(avgVec);
            end
end

%createVirtualPilots Create virtual pilots.
%   [VP] = createVirtualPilots(..) returns a 3xNvp matrix which contains 
%   the time/frequency and least squares estimate of virtual pilots.
%   Nvp is the number of virtual pilots.
%
%   The inputs are:
%
%   ENB structure which must contain the following fields:
%   NDLRB           - Number of downlink resource blocks
%   CyclicPrefix    - Optional. Cyclic prefix length {'Normal'(default),'Extended'}   
%
%   P_EST - A matrix containing the pilots within the grid. The matrix must
%   have three rows. Each column contains the information for each pilot:
%   [subcarrier_indices symbol_indices pilot_channel_estimate]
%
%   ANTENNA - The current antenna port
%   CREATE_TOP,CREATE_BOTTOM,CREATE_FRONT,CREATE_END - a value {0 or 1}
%   indicating whether to return virtual pilots in each position. A 1
%   indicates virtual pilots should be returned, a 0 indicates they should
%   not be returned.
% 
%   Example:
%
%   Create virtual pilots on the top of a grid of pilots (estimates)
%   VP_TOP = createVirtualPilots(ENB,estimates,0,1,0,0,0);
%
%   Add virtual pilots to current pilot estimates for interpolation
%   estimates = [estimates VP_TOP];

%   Copyright 2009-2010 The MathWorks, Inc.
function VP = createVirtualPilots(ENB,P_EST,CREATE_TOP,CREATE_BOTTOM,CREATE_FRONT,CREATE_END)
    
    % Initialize outputs
    VP = zeros(3,0);
    
    % Get resource grid dimensions
    dims = lteDLResourceGridSize(ENB);
    K = dims(1);

    % Virtual pilot cut-off values - indices in time or frequency at which
    % the virtual pilot shall be discarded. As the symbol indices of the
    % pilots passed in may be positive and negative the start and end of
    % the effective grid varies therefore affecting the front and end pilot
    % cut-off values
    coTop = -6; %
    coBottom = K+6; %
    coFront = min(P_EST(2,:))-7; % floor(min(P_EST(2,:))/L)*L-5; %
    coEnd = max(P_EST(2,:))+7; % ceil(max(P_EST(2,:))/L)*L+5;

    % Number of pilots in a subcarrier is dependent upon antenna
%     noPilotinSC = (2-floor(ANTENNA/2))*nSF;
    noPilotinSC = numel(P_EST(P_EST(1,:)==P_EST(1,1)));
    
    % Number of pilots in a symbol
    noPilotinSym = K/6;

    % Sort by subcarrier
    [~, indices] = sortrows(P_EST(1,:).');
    p_est_sorted_sc = P_EST(:,indices).';
 
    if CREATE_TOP
        % Repeat first and second SC containing pilots
        p_est_rep_above = p_est_sorted_sc(1+noPilotinSC:noPilotinSC*2,:);
        p_est_rep_above = [p_est_rep_above.' p_est_sorted_sc(1:noPilotinSC,:).'].'; 

        % Subtract six to normalize subcarrier indices
        p_est_rep_above(:,1) = p_est_rep_above(:,1) - 6;

        % Remove virtual pilots which are too far
        p_est_rep_above(p_est_rep_above(:,1) < coTop,:) = [];

        % Initialize vp vector
        VP_TOP = zeros(size(p_est_rep_above));

        % Create virtual pilots on top
        for p = 1:size(p_est_rep_above)
            VP_TOP(p,:) = calculatePilot(p_est_rep_above(p,:), p_est_sorted_sc);
        end

        % Transpose to make suitable for output
        VP = [VP VP_TOP.'];
    end

    if CREATE_BOTTOM
        % Repeat last and second last SC containing pilots
        p_est_rep_below = p_est_sorted_sc(end-noPilotinSC*2+1:end-noPilotinSC,:);
        p_est_rep_below = [p_est_rep_below.' p_est_sorted_sc(end-noPilotinSC+1:end,:).'].';

        % Add six to normalize subcarrier indices
        p_est_rep_below(:,1) = p_est_rep_below(:,1) + 6;   

        % Remove virtual pilots which are too far
        p_est_rep_below(p_est_rep_below(:,1) > coBottom,:) = [];

        % Initialize vp vector
        VP_BOTTOM = zeros(size(p_est_rep_below));

        % Create virtual pilots on bottom
        for p = 1:size(p_est_rep_below)
            VP_BOTTOM(p,:) = calculatePilot(p_est_rep_below(p,:), p_est_sorted_sc);
        end

        % Transpose to make suitable for output
        VP = [VP VP_BOTTOM.'];
    end

    % Sort by symbol
    [~, indices] = sortrows(P_EST(2,:).');
    p_est_sorted_sym = P_EST(:,indices).';

    if CREATE_FRONT
        % Repeat first symbol containing pilots
        p_est_rep_front = p_est_sorted_sym(noPilotinSym+1:(noPilotinSym*2),:);

        % Add subcarrier above and below
        p_est_rep_front = addSCs(p_est_rep_front,noPilotinSym);

        % Repeat second symbol containing pilots
        p_est_rep_front_second = p_est_sorted_sym(1:noPilotinSym,:);

        % Add subcarrier above and below
        p_est_rep_front_second = addSCs(p_est_rep_front_second,noPilotinSym);

        % Concatenate to create virtual pilots
        p_est_rep_front = [p_est_rep_front.' p_est_rep_front_second.'].';

        % Subtract seven to normalize symbol indices
        p_est_rep_front(:,2) = p_est_rep_front(:,2) - 7;  

        % Remove virtual pilots which are too far
        p_est_rep_front = removeTooFar(p_est_rep_front,p_est_sorted_sym,coTop,coBottom,coFront,coEnd);

        % Initialize vp vector
        VP_FRONT = zeros(size(p_est_rep_front));

        % Create virtual pilots on front
        for p = 1:length(p_est_rep_front)
            VP_FRONT(p,:) = calculatePilot(p_est_rep_front(p,:), p_est_sorted_sc);
        end

        % Transpose for output      
        VP = [VP VP_FRONT.'];
    end

    if CREATE_END
        % Repeat last symbol containing pilots
        p_est_rep_end = p_est_sorted_sym(end-(noPilotinSym*2)+1:end-noPilotinSym,:);

        % Add subcarrier above and below
        p_est_rep_end = addSCs(p_est_rep_end,noPilotinSym);

        % Repeat second symbol containing pilots
        p_est_rep_end_second = p_est_sorted_sym(end-noPilotinSym+1:end,:);

        % Add subcarrier above and below
        p_est_rep_end_second = addSCs(p_est_rep_end_second,noPilotinSym);

        % Concatenate to create virtual pilots
        p_est_rep_end = [p_est_rep_end.' p_est_rep_end_second.'].';

        % Add seven to normalize symbol indices, mod controls case when
        % only 2 pilots per subcarrier
        p_est_rep_end(:,2) = p_est_rep_end(:,2) + 7;  

        % Remove virtual pilots which are too far
        p_est_rep_end = removeTooFar(p_est_rep_end,p_est_sorted_sym,coTop,coBottom,coFront,coEnd);

        % Initialize vp vector
        VP_END = zeros(size(p_est_rep_end));

        % Create virtual pilots on front
        for p = 1:length(p_est_rep_end)
            VP_END(p,:) = calculatePilot(p_est_rep_end(p,:), p_est_sorted_sc);
        end

        % Transpose for output
        VP = [VP VP_END.'];
    end
end

function vp = calculatePilot(p_est_rep, p_est_sorted)
    % Calculate Euclidean distance between virtual pilot and other
    % pilots 
    ind = sqrt((p_est_rep(1)-p_est_sorted(:,1)).^2+ (p_est_rep(2)-p_est_sorted(:,2)).^2);
    % ind = (abs(p_est_rep(1)-p_est_sorted(:,1))+ abs(p_est_rep(2)-p_est_sorted(:,2)));

    % Sort from shortest to longest distance
    [~, ind] = sortrows(ind);

    % Take three closest pilots
    ind = ind(1:10);
    pilots_use = p_est_sorted(ind,:);

    % If first 3 pilots used are in the same subcarrier or symbol then use 4th instead of 3rd
    while ((pilots_use(3,1) == pilots_use(2,1)) && (pilots_use(2,1) == pilots_use(1,1)) || (pilots_use(3,2) == pilots_use(2,2)) && (pilots_use(2,2) == pilots_use(1,2)))
        pilots_use(3,:) = [];
    end

    % Calculate virtual pilot value
    vp = calculateVirtualValue(pilots_use(3,:),pilots_use(1,:),pilots_use(2,:),p_est_rep(1),p_est_rep(2));
end

function new = calculateVirtualValue(a,b,c,xnew,ynew)
    % Calculate vectors
    AB = b-a;
    AC = c-a; 

    % Perform cross product
    cro = cross(AB,AC);
    
    % Break out X, Y and Z plane coeffs
    x = cro(1);
    y = cro(2);
    z = cro(3);

    % Calculate normal in equation
    c = sum(cro.*a);

    % Calculate new z value
    znew = (c - xnew*x - ynew*y)/z;
    
    % Return new point
    new = [xnew ynew znew];
end

function pilots = addSCs(pilots, noPilotinSym)
    % Add on extra subcarrier above
    pilots(end+1,:) = pilots(1,:);
    pilots(end,1) = pilots(end,1) - 6;

    % Add on extra subcarrier below
    pilots(end+1,:) = pilots(noPilotinSym,:);
    pilots(end,1) = pilots(end,1) + 6;
end

function pilots = removeTooFar(pilots,p_est_sorted_sym,coTop,coBottom,coFront,coEnd)
    pilots((pilots(:,2) < coFront) | (pilots(:,1) < coTop) | (pilots(:,2)> coEnd) | (pilots(:,1) > coBottom),:) = [];

    % Remove virtual pilots which fall within resource grid
    removeInd = [];
    for i = 1:size(pilots)
        if (find(p_est_sorted_sym(:,2) == pilots(i,2)))
            removeInd = [removeInd i]; %#ok<AGROW>
        end
    end
    pilots(removeInd,:) = [];    
end

%CreateEdgeVirtualPilots 
% Creates virtual pilots on the edges of the resource grid to improve
% interpolation results. Also adds virtual pilots around the current pilot
% estimates.
function vps = createEdgeVirtualPilots(enb,p_est)

    % Determine dimensions of current subframe
    dims=lteDLResourceGridSize(enb);
    K=dims(1);
    L=dims(2);
            
    % Calculate virtual pilots beyond upper and lower bandwidth edge based
    % on pilots present on subcarriers. Also create virtual pilots 1/2 RB
    % above and below extent of current pilots. 
    vps=createDimensionVirtualPilots(p_est,2,unique([-6 min(p_est(1,:)-6) max(p_est(1,:)+6) K+6]));                    
        
    % Combine these frequency-direction VPs with original pilots.
    temp = [p_est vps];
    
    % Calculate virtual pilots beyond start and end of subframe based on
    % pilots and frequency-direction VPs present in symbols. Also create 
    % virtual 1 OFDM symbol before and after extent of current pilots. 
    vps = [vps createDimensionVirtualPilots(temp,1,unique([-1 min(p_est(2,:)-1) max(p_est(2,:)+1) L+1]))];       
    
end

% dim=1 adds VPs in time
% dim=2 adds VPs in frequency
function vps = createDimensionVirtualPilots(p_est,dim,points)

    vps=[];        
    
    pilots = unique(p_est(dim,:));        
    for i = pilots
        x=find(p_est(dim,:)==i);
        rep=1+double(length(x)==1);
        adj=(1:rep)-rep;
        pil = interp1(p_est(3-dim,x)+adj,repmat(p_est(3,x),1,rep),points,'linear','extrap');
        for j=1:length(points)
            vps = [ vps [i;points(j);pil(j)] ];         %#ok<AGROW>
        end
    end
    
    if (dim==2)
        vps = vps([2 1 3],:);
    end
    
end


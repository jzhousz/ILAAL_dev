function WaveStatistics(image_path, save_path, miny, maxy, Light)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
%clear all

%Ref: http://www.mathworks.com/help/wavelet/ref/cwt.html
%adaptive wavelet thresholding

% Videos

%image_path = 'D:\research\waveAnalysis\exploration_data\1X\experimentalGroup\002\ProjMax_ch01.tif';
%save_path = 'D:\research\waveAnalysis\exploration_data\1X\experimentalGroup\002';

        % ProjMax_ch01.tif
        OPTLightTime = Light;
        neuropil_miny = double(miny);   
        neuropil_maxy =  double(maxy);
        segmentxarray = [121 172 219 259 301 340 375 415];
        yticklabels =  1:8;
        FileTif = (image_path);
        %waveImage    = fullfile(save_path,waveImage.jpg);
        heatmapImage = fullfile(save_path,'heatmapImage');
        %waveOfSec = fullfile(save_path,waveOfSec);
        %peaksDec = fullfile(save_path,peak);
        peakMap = fullfile(save_path,'DetectedPeaks');
        ExperNum = length(segmentxarray); % Section done to test peak detection
        filename = fullfile(save_path,'Wave_Statistics.xlsx');
        if exist(filename,'file')
            EraseExcelSheets(filename);%% erase existing content if file exist   
        end
            
            
        
        

segmentHalfWidth = 11;

% Reading the Video into Matlab
 InfoImage=imfinfo(FileTif);
 mImage=InfoImage(1).Width;
 nImage=InfoImage(1).Height;
NumberImages=length(InfoImage);
for i= 1:NumberImages
    vidFrames(:,:,i) = imread(FileTif,'Index',i);
end

%% Smoothing frames
window_size = 20;       

for avg_frame = 1 : NumberImages
    fAverage(avg_frame) = mean2(vidFrames(:,:,avg_frame));
end

for F1_frame = 1 : NumberImages
    vectorstart = max(1, F1_frame - window_size/2);
    vectorend   = min(NumberImages, F1_frame + window_size/2);
    vector = fAverage(vectorstart:vectorend);
    F1(F1_frame) = mean(vector);
end


%% Calculating time dependent baseline F0

%testRangeF0 = 10;

% positionCount = 1;
% for F0_frame = 1 : NumberImages
%     if positionCount > testRangeF0
%         beforetenNumberList = fAverage(positionCount-testRangeF0:positionCount);
%             F0(positionCount) = min(beforetenNumberList);
%     end
%     positionCount = positionCount+1;      
% end

t2 = 20;
for F0_frame = 1:NumberImages
    vectorstart = max(1, F0_frame-t2);
    vector = F1(vectorstart:F0_frame);
    F0(F0_frame) = min(vector);
end

%% Calculate deltaF/F0


for rel = 1: NumberImages
    relative(:,:,rel) = imsubtract(vidFrames(:,:,rel),F0(rel));
    relative(:,:,rel) = relative(:,:,rel)./F0(rel);
end


%% denoising

% t0 = 10;
% denominator = 0;
% 
% for i = 1:NumberImages
%   % e^(- t/t0)
%   denominator = denominator + exp(-1 * i/t0);
%   nominator = 0;
%   for j = 0:i-1
%     nominator = nominator + relative(:,:,i-j)*exp(-1*j/t0);
%   end
%   denoised(:,:,i) = nominator/denominator;
% end  

%% Heatmap

segmentWidth = segmentHalfWidth*2 +1;
totalpixelsPerSegment = (neuropil_maxy - neuropil_miny + 1 ) * segmentWidth;
startFrame = 1;
endFrame = NumberImages;

heatmapdata = zeros(size(segmentxarray,2) , endFrame-startFrame+1);
for segmentindex = 1 : size(segmentxarray,2)
    % response is the signal for peak detection.
    response = zeros(endFrame-startFrame+1,1);
    for k = 1 : (endFrame-startFrame+1)
        for y = neuropil_miny : neuropil_maxy
            for x =  segmentxarray(segmentindex) - segmentHalfWidth : segmentxarray(segmentindex) + segmentHalfWidth  %current segment (roughly)
                response(k) = response(k) + int64(relative(y, x, k+startFrame-1));      % NOTE:  x and y are switched due to Matlab's storage format!
            end
        end
                 
        response(k) = response(k)./totalpixelsPerSegment;
                  
    end
    % prepare the data for heatmap
    heatmapdata(segmentindex, :) = response';
    % plot the response signal for current segment
    x = startFrame:endFrame;
    
end

% draw the signal on heatmap
h2=figure
colormap('hot');
imagesc(heatmapdata);

%customize label for the heatmap
xticklabels = 1:10:endFrame;
xticks = 1:10:endFrame;
set(gca, 'XTick', xticks, 'XTickLabel', xticklabels)
yticks = linspace(1, size(heatmapdata, 1), numel(yticklabels));
set(gca, 'YTick', yticks, 'YTickLabel', yticklabels)
xlabel('time [frame]'); ylabel('Calcium Response In Segment');
colorbar;
 %set(gca, 'clim', [0 160]);
saveas(h2, strcat( heatmapImage, '.jpg' ), 'jpg');

    


%% PeakMap
peaks = zeros(ExperNum,NumberImages, 'double');
zeroCross = zeros(ExperNum,NumberImages, 'double');

for secNum = 1 : ExperNum 
    %Staring Vars
    
    pos = zeros(1,NumberImages, 'double');
    segOfInt = heatmapdata( secNum, : );
    %rbioSet= cellstr( [ 'rbio1.1'; 'rbio1.3'; 'rbio1.5';
    %    'rbio2.2'; 'rbio2.4'; 'rbio2.6'; 'rbio2.8';
    %    'rbio3.1'; 'rbio3.3'; 'rbio3.5'; 'rbio3.7';
    %    'rbio3.9';'rbio4.4';'rbio5.5'; 'rbio6.8';]);
    
    
    %for rbioNum = 1 : 15
    
    %ccfsMtx = cwt(segOfInt,2:6,char(rbioSet(rbioNum))); %rbio1.1, mexh
    ccfsMtx = cwt(segOfInt,2:6,'rbio1.1');
    %Searchs thought all sections for peaks
    for k = 1 : 5
        for i = 1 : NumberImages
            if i + 1 < NumberImages
                if ccfsMtx(k,i) < 0 && ccfsMtx(k,i + 1) >= 0
                    pos(i) = pos(i) + 1;
                    pos(i+1) = pos(i+1) + 1;
                    if i - 1 > 0
                        pos(i-1) = pos(i-1) + 1;
                    end
                end
            end
        end
    end
    %end
    zeroCross(secNum,:) = pos;
    
    
    %Reduces false postives with thrshold teqnue
    for window = 15 : NumberImages % dynamic window size, will change window size each iteration
        segOfprocess = heatmapdata( secNum, : );
        segOfInt = heatmapdata( secNum, : );
        for cnt = 1 : NumberImages
            if cnt - window <= 0 %14
                leftIdx = 1;
                if cnt + window > NumberImages
                    rightIdx = NumberImages;
                else
                    rightIdx = cnt + window; %+ 2;
                end
            else
                if cnt + window >= NumberImages
                    rightIdx = NumberImages;
                    leftIdx = cnt - window;% + 2;
                else
                    rightIdx = cnt + window;
                    leftIdx = cnt - window;
                end
            end
            meanTol = mean2(segOfprocess(leftIdx:rightIdx));
            
            if segOfInt(cnt) - ( meanTol + (meanTol * .05)) < 0
                segOfInt(cnt) = 0;
            else
                segOfInt(cnt) = segOfInt(cnt) - ( meanTol + (meanTol * .05));
            end
        end
        
        %From the normaliztion, peaks will be picked based on threshold and
        %wavelet informaton
        mean_new = mean2(segOfInt);
        for i = 1 : NumberImages
            if pos(i) > 2 && segOfInt(i) > (mean_new)
                if i - 1 > 0 && i + 1 < NumberImages + 1
                    if segOfInt(i) > segOfInt(i + 1) && segOfInt(i) > segOfInt(i - 1)% && segOfInt(i) > max(segOfInt) / 8 % 6 best
                        if i - 1 > 0 && i + 1 <= NumberImages
                            if peaks(secNum, i -1) == 0 && peaks(secNum, i +1) == 0
                                peaks(secNum, i) = segOfInt(i);
                            end
                        else
                            peaks(secNum, i) = segOfInt(i);
                        end
                    end
                end
            end
        end
    end
    
    %Makes the peaks the correct size for graphing
    segOfInt = heatmapdata( secNum, : );
    
    for i = 1 : NumberImages
        if peaks(secNum, i) > 0
            peaks(secNum, i) = segOfInt(i); %10
        end
    end
    
    %      currentPeaks = peaks(secNum, :);
    %      currentPeaks (currentPeaks == 0 ) = NaN;
    %      h4 = figure
    %      plot( x, segOfInt, x, currentPeaks, '*')
    %      axis([startFrame endFrame 0 max(segOfInt)+10]);
    %     saveas(h4, strcat( strcat(peaksDec, num2str(secNum)), '.jpg' ), 'jpg');
end






%Graphs each segments peaks found
for secNum = 1 : ExperNum
    currentPeaks = peaks(secNum, :);
    peaks_to_print = peaks;
   % disp(strcat ({'Amplitude: '} , num2str(peaks(secNum,:))))
    currentPeaks (currentPeaks == 0 ) = NaN;
   % h4 = figure
    close all
    plot( x, heatmapdata(secNum, :), x, currentPeaks, '*')
    axis([startFrame endFrame 0 max(heatmapdata(secNum, :))+10]);
%     saveas(h4, strcat( strcat(peaksDec, num2str(secNum)), '.jpg' ), 'jpg');
end

%Graphs the peak map
peaks( peaks > 0 ) = 1;
h5=figure

colormap('hot');
imagesc(peaks);
xticklabels = startFrame:10:endFrame;
xticks = startFrame:10:endFrame;
set(gca, 'XTick', xticks, 'XTickLabel', xticklabels)
yticks = linspace(1, size(heatmapdata, 1), numel(yticklabels));
set(gca, 'YTick', yticks, 'YTickLabel', yticklabels)
xlabel('time [frame]'); ylabel('Peaks In Segment');
colorbar;
saveas(h5, strcat( peakMap, '.jpg' ), 'jpg');
close all


%display to output info
Segloction = zeros(1,NumberImages, 'int32');
SegValue = zeros(1,NumberImages, 'int32');
direction = zeros(1,NumberImages, 'int32'); % 1 left, 2 middle, 3 right, 4 start
cnt = 1;
wave_count = 1;
disp('--------------------------------------------------------------------------------------------------------------------------')




for secNum = 1 : ExperNum
    
    for i = 1 : NumberImages
        if peaks(secNum, i ) == 1 % Starting peaks

            Segloction(cnt) = secNum;
            SegValue(cnt) = i;
            direction(cnt) = 4;
            peaks(secNum, i ) = 0;
            nextSec = secNum+1;
            nextI = i;
            cnt = cnt + 1;
            while nextSec > 0 && nextSec <= ExperNum && nextI + 1 < NumberImages && nextI - 1 > 0
                
                if peaks( nextSec, nextI ) == 1 % If a peak is not moving left or right
                    peaks( nextSec, nextI ) = 0;
                    Segloction(cnt) = nextSec;
                    SegValue(cnt) = nextI;
                    direction(cnt) = 2;
                    nextSec = nextSec+1;
                    cnt = cnt + 1;
                elseif isempty(direction(direction == 1 )) && nextI + 5 < NumberImages % if a peaks is 5 to the right of current peak
                    if sum(peaks( nextSec, nextI + 1 : nextI + 5  )) > 0
                        [ val, loc ] = max(peaks( nextSec, nextI + 1 : nextI + 5  ));
                        if  peaks( nextSec - 1, nextI + loc ) == 0 || loc <=3 % add a condition to allow more jumps
                            peaks( nextSec, nextI + loc ) = 0;
                            Segloction(cnt) = nextSec;
                            SegValue(cnt) = nextI + loc;
                            direction(cnt) = 3;
                            nextSec = nextSec+1;
                            nextI = nextI+loc;
                            cnt = cnt + 1;
                        else
                            nextSec = 0;
                        end
                    elseif isempty(direction(direction == 3 )) && nextI - 5 > 0 % if a peaks is 5 to the left of current peak
                        if sum(peaks( nextSec, nextI - 5 : nextI - 1  )) > 0
                            flippedLeftPeaks = fliplr(peaks( nextSec, nextI - 5 : nextI - 1  ));
                            [ val, loc ] = max(flippedLeftPeaks); 
                            loc = 6-loc;
                            if  peaks( nextSec - 1, nextI - (6 - loc) ) == 0 ||loc >=3 %add a condition to allow more jumps
                                peaks( nextSec, nextI - (6 - loc) ) = 0;
                                Segloction(cnt) = nextSec;
                                SegValue(cnt) = nextI -(6 - loc);
                                direction(cnt) = 1;
                                nextSec = nextSec+1;
                                nextI = nextI-(6 - loc);
                                cnt = cnt + 1;
                            else
                                nextSec = 0;
                            end
                        else
                            nextSec = 0;
                        end
                    else
                        nextSec = 0;
                    end
                elseif isempty(direction(direction == 3 )) && nextI - 5 > 0 % if a peaks is 5 to the left of current peak
                    if sum(peaks( nextSec, nextI - 5 : nextI - 1  )) > 0
                        flippedLeftPeaks = fliplr(peaks( nextSec, nextI - 5 : nextI - 1  )); 
                        [ val, loc ] = max(flippedLeftPeaks);  %get the location that is closer to nextI
                        loc = 6 - loc; % flip the location back to be aliged with original logic
                        if  peaks( nextSec - 1, nextI - (6 - loc) ) == 0 || loc >= 3
                            peaks( nextSec, nextI - (6 - loc) ) = 0;
                            Segloction(cnt) = nextSec;
                            SegValue(cnt) = nextI -(6 - loc);
                            direction(cnt) = 1;
                            nextSec = nextSec+1;
                            nextI = nextI-(6 - loc);
                            cnt = cnt + 1;
                        else
                            nextSec = 0;
                        end
                    else
                        nextSec = 0;
                    end
                else
                    nextSec = 0;
                end
            end
        end
        cnt = 1;
        
      
      num_of_seg = nnz(Segloction(Segloction >0));  %% Varshini
      if(num_of_seg > 1)            %% Varshini
      
        if ~isempty(Segloction(Segloction >0))
            
            [ fTime, loc ] = min(SegValue(SegValue >0));
            if loc < length(SegValue(SegValue >0)) && loc ~= 1
                while SegValue(loc + 1 ) == fTime && loc < length(SegValue(SegValue >0))
                    loc = loc + 1;
                end
            end
            
            segment_location = Segloction(loc) + yticklabels(1) - 1;
            firing_time_loc =  fTime;
            segAnn = SegValue(SegValue >0);
            peak_segment = Segloction(Segloction>0);
            
            
            
            for peak_count = 1:num_of_seg
                peakAmplitude(peak_count) = peaks_to_print(peak_segment(peak_count),segAnn(peak_count));
            end
            
           medianAmplitude =  median(peakAmplitude);
            
            
            
            disp('Wave Starting Point ')
            disp( strcat ({'Firing Time: '} , num2str(fTime),{' Segment Location: '}, num2str(Segloction(loc) + yticklabels(1) - 1)))
            disp('Wave Information Of Segments')
            disp( strcat ({'Segment Locations: '} , num2str(Segloction(Segloction >0) + yticklabels(1) - 1)))
            disp( strcat ({'Firing Times: '} , num2str(SegValue(SegValue >0))))
            disp( strcat ({'Amplitude: '} , num2str(peakAmplitude)))
            disp( strcat ({'Segment Direction: '}, num2str(direction(direction >0))))
            disp( {'Segment Direction Key: 1 left, 2 middle, 3 right, 4 start'})
            
            segment_loc_wave = (Segloction(Segloction >0) + yticklabels(1) - 1);
            
            
            disp('Wave Information')
            disp(strcat({'Duration Of Wave: '}, num2str(abs(segAnn(1) - segAnn(length(segAnn)))), {' Seconds'}))
            %***************** Varshini*********************** 
            
            Dur_of_wave = abs(segAnn(1) - segAnn(length(segAnn)));
            wave_speed = double(num_of_seg) / double(Dur_of_wave);
            
            if(Dur_of_wave > 0)
                excel_speed = wave_speed;
            else
                excel_speed = 'NAN';
            end
            
            
            if(Dur_of_wave > 0 )
                disp(strcat ({'Wave Speed: '} , num2str(wave_speed), {' Segments/sec'}))
     
            else
                disp('Wave Speed : None (Wave has no duration)')
                
            end
            %*************************************************
             %disp( strcat ({'Wave Speed: '} , num2str((nnz(Segloction(Segloction >0)))  / (abs(segAnn(1) - segAnn(length(segAnn))))), {' Seg/sec'}))
            if segAnn(1) - segAnn(length(segAnn)) > 0
                wave_direction = 'P->A';
                disp('Wave Direction: Posterior To Anterior ')
            elseif  segAnn(1) - segAnn(length(segAnn)) < 0
                wave_direction = 'A->P';
                disp('Wave Direction: Anterior To Posterior')
            else
                wave_direction = 'Unknown';
                disp('Wave Direction: Unknown')
            end
            %display(Segloction(Segloction >0))
            %display(SegValue(SegValue >0))
            %display(direction(direction >0))
            
            xlsinput2 = {'Firing_Time', 'Segment_location_of_firing','Duration_of_wave(Frames)','Number_of_Segments_covered', 'Wave_Speed(Segments/frame)', 'Amplitude(median)', 'Wave_direction';firing_time_loc, segment_location, Dur_of_wave, num_of_seg, excel_speed, medianAmplitude, wave_direction};
           
            sheet = 1;
            xlRange1 = sprintf( 'A%s',num2str((wave_count*2)-1) );
            xlswrite(filename,xlsinput2,sheet,xlRange1); 
            
            %sorting the values of firing times and put them into array
            firing_times_array(wave_count) = fTime; 
            

            
            if sum(SegValue) > 0
                disp('--------------------------------------------------------------------------------------------------------------------------')
                wave_count = wave_count +1;
                
            end
            
            sorted_firing_times_array = sort(firing_times_array);
            afterOPTLightWaveIndex = find(sorted_firing_times_array > OPTLightTime, 1, 'first');
            afterOPTLightWave = sorted_firing_times_array(afterOPTLightWaveIndex);
            
            xlsinput1 = {'Total_number_of_waves', 'First_Wave_firing_time_after_light', 'OptogeneticLight_application_time';wave_count-1,afterOPTLightWave, OPTLightTime};
            sheet = 1;
            xlRange2 = sprintf( 'A%s',num2str((wave_count*2)-1));
            xlswrite(filename,xlsinput1,sheet,xlRange2);
           disp('DONE!')
           peakAmplitude = [];
        end
      end
        Segloction = zeros(1,NumberImages, 'int32');
        direction = zeros(1,NumberImages, 'int32');
        SegValue = zeros(1,NumberImages, 'int32');
         
    end
end
% peaks( peaks > 0 ) = 1;
% h6=figure
% colormap('hot');
% imagesc(peaks);




end
%% Function to create new excel sheet every time 

function EraseExcelSheets(fileName)
% By ImageAnalyst
% Check whether the file exists
if ~exist(fileName,'file')
error([fileName ' does not exist !']);
else
% Check whether it is an Excel file
typ = xlsfinfo(fileName);
if ~strcmp(typ,'Microsoft Excel Spreadsheet')
error([fileName ' not an Excel sheet !']);
end
end

% If fileName does not contain a "\" the name of the current path is added
% to fileName. The reason for this is that the full path is required

%for the command "excelObj.workbooks.Open(fileName)" to work properly.
if isempty(strfind(fileName,'\'))
fileName = [cd '\' fileName];
end

excelObj = actxserver('Excel.Application');
excelWorkbook = excelObj.workbooks.Open(fileName);
worksheets = excelObj.sheets;
numSheets = worksheets.Count;
% Prevent beeps from sounding if we try something not allowed.
excelObj.EnableSound = false;

% Loop over all sheets
    for s = 1 : numSheets
        % worksheets.Item(sheetIndex).UsedRange is the range of used cells.
        worksheets.Item(s).UsedRange.Delete;
    end
excelObj.EnableSound = true;
excelWorkbook.Save;
excelWorkbook.Close(false);
excelObj.Quit;
delete(excelObj);
return;
end

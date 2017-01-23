function varargout = WaveFinderGUI(varargin)
% WAVEFINDERGUI MATLAB code for WaveFinderGUI.fig
%      SIMPLE_G5UI, by itself, creates a new WAVEFINDERGUI or raises the existing
%      singleton*.
%
%      H = WAVEFINDERGUI returns the handle to a new WAVEFINDERGUI or the handle to
%      the existing singleton*.
%
%      WAVEFINDERGUI('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in WAVEFINDERGUI.M with the given input arguments.
%
%      WAVEFINDERGUI('Property','Value',...) creates a new WAVEFINDERGUI or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before WaveFinderGUI_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to WaveFinderGUI_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help WaveFinderGUI

% Last Modified by GUIDE v2.5 19-Jan-2017 16:21:42

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @WaveFinderGUI_OpeningFcn, ...
                   'gui_OutputFcn',  @WaveFinderGUI_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before WaveFinderGUI is made visible.
function WaveFinderGUI_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to WaveFinderGUI (see VARARGIN)

% Choose default command line output for WaveFinderGUI
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);


% UIWAIT makes WaveFinderGUI wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = WaveFinderGUI_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes when figure1 is resized.
function figure1_SizeChangedFcn(hObject, eventdata, handles)
% hObject    handle to figure1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)


function min_Callback(hObject, eventdata, handles)
% hObject    handle to min (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of min as text
%        str2double(get(hObject,'String')) returns contents of min as a double

min_value = str2double(get(hObject,'String'));
global min
min = min_value;
%min_value = min_value;
%guidata(hObject, handles);

%if (isempty(min_value))
%     set(hObject,'String','0')
%end



% --- Executes during object creation, after setting all properties.
function min_CreateFcn(hObject, eventdata, handles)
% hObject    handle to min (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function max_Callback(hObject, eventdata, handles)
% hObject    handle to max (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of max as text
%        str2double(get(hObject,'String')) returns contents of max as a double

max_value = str2double(get(hObject,'String'));
%handles.max_value = max_value;
global max
max = max_value;

%if (isempty(max_value))
%     set(hObject,'String','0')
%end

%guidata(hObject, handles)


% --- Executes during object creation, after setting all properties.
function max_CreateFcn(hObject, eventdata, handles)
% hObject    handle to max (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



% --- Executes on button press in cmd_getDir.
function cmd_getDir_Callback(hObject,eventdata, handles)
% hObject    handle to cmd_getDir (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% Browse for the file. Change starting folder as necessary.
folder = 'C:\Users\Acer\Documents\MATLAB';
if ~exist(folder, 'dir')
	folder = pwd;
end
fileSpecification = [folder '/*.tif*'];
% Ask user to specify the file.
[baseFileName, folder] = uigetfile(fileSpecification, 'Specify a file'); 
if baseFileName == 0
	return;
end
% Build up the full file name.
fullFileName = fullfile(folder, baseFileName); 
set(handles.txt_directory,'String',fullFileName)
global folder_path1
folder_path1 = fullFileName;


% --- Executes on button press in cmd_getDirectory2.
function cmd_getDirectory2_Callback(hObject,eventdata, handles)
% hObject    handle to cmd_getDirectory2 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
folder = 'C:\';


folder_name = uigetdir(folder, 'Select directory to open') ;


set(handles.text4,'String',folder_name)

global folder_path2
folder_path2 = folder_name;



% --- Executes on button press in pushbutton1.
function pushbutton1_Callback(hObject,eventdata, handles)
% hObject    handle to pushbutton1 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

global folder_path2
global folder_path1
global opt_path3
global min
global max



WaveStatistics(folder_path1,folder_path2,opt_path3,min, max)



% --- Executes during object creation, after setting all properties.
function light_CreateFcn(hObject, eventdata, handles)
% hObject    handle to light (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end
OptogeneticLight = str2double(get(hObject,'String'));
global opt_path3
opt_path3 = OptogeneticLight;

% --- Executes on button press in cmd_getDir3.
function cmd_getDir3_Callback(hObject, eventdata, handles)
% hObject    handle to cmd_getDir3 (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
folder = 'C:\Users\Acer\Documents\MATLAB';
if ~exist(folder, 'dir')
	folder = pwd;
end
fileSpecification = [folder '/*.tif*'];
% Ask user to specify the file.
[baseFileName, folder] = uigetfile(fileSpecification, 'Specify a file'); 
if baseFileName == 0
	return;
end
% Build up the full file name.
fullFileName = fullfile(folder, baseFileName); 
set(handles.txt_directory3,'String',fullFileName)
global opt_path3
opt_path3 = fullFileName;

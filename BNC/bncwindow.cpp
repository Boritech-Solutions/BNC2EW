// Part of BNC, a utility for retrieving decoding and
// converting GNSS data streams from NTRIP broadcasters.
//
// Copyright (C) 2007
// German Federal Agency for Cartography and Geodesy (BKG)
// http://www.bkg.bund.de
// Czech Technical University Prague, Department of Geodesy
// http://www.fsv.cvut.cz
//
// Email: euref-ip@bkg.bund.de
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, version 2.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

/* -------------------------------------------------------------------------
 * BKG NTRIP Client
 * -------------------------------------------------------------------------
 *
 * Class:      bncWindow
 *
 * Purpose:    This class implements the main application window.
 *
 * Author:     L. Mervart
 *
 * Created:    24-Dec-2005
 *
 * Changes:    
 *
 * -----------------------------------------------------------------------*/

#include <iostream>

#include <unistd.h>
#include "bncwindow.h" 
#include "bncapp.h" 
#include "bncgetthread.h" 
#include "bnctabledlg.h" 
#include "bncipport.h" 
#include "bnchlpdlg.h" 
#include "bnchtml.h" 
#include "bnctableitem.h"
#include "bncsettings.h"

using namespace std;

// Constructor
////////////////////////////////////////////////////////////////////////////
bncWindow::bncWindow() {

  _caster = 0;

  int ww = QFontMetrics(this->font()).width('w');
  
  static const QStringList labels = QString("account, Streams:     broadcaster:port/mountpoint,decoder,lat,long,nmea,ntrip,bytes").split(",");

  setMinimumSize(85*ww, 65*ww);

  setWindowTitle(tr("BKG Ntrip Client (BNC) Version 1.7"));

  connect((bncApp*)qApp, SIGNAL(newMessage(QByteArray,bool)), 
           this, SLOT(slotWindowMessage(QByteArray,bool)));

  QPalette palette;
  QColor lightGray(230, 230, 230);

  // Create Actions
  // --------------
  _actHelp = new QAction(tr("&Help Contents"),this);
  connect(_actHelp, SIGNAL(triggered()), SLOT(slotHelp()));

  _actAbout = new QAction(tr("&About BNC"),this);
  connect(_actAbout, SIGNAL(triggered()), SLOT(slotAbout()));

  _actFlowchart = new QAction(tr("&Flow Chart"),this);
  connect(_actFlowchart, SIGNAL(triggered()), SLOT(slotFlowchart()));

  _actFontSel = new QAction(tr("Select &Font"),this);
  connect(_actFontSel, SIGNAL(triggered()), SLOT(slotFontSel()));

  _actSaveOpt = new QAction(tr("&Save && Activate Options"),this);
  connect(_actSaveOpt, SIGNAL(triggered()), SLOT(slotSaveOptions()));

  _actQuit  = new QAction(tr("&Quit"),this);
  connect(_actQuit, SIGNAL(triggered()), SLOT(close()));

  _actAddMountPoints = new QAction(tr("Add &Streams"),this);
  connect(_actAddMountPoints, SIGNAL(triggered()), SLOT(slotAddMountPoints()));

  _actDeleteMountPoints = new QAction(tr("&Delete Streams"),this);
  connect(_actDeleteMountPoints, SIGNAL(triggered()), SLOT(slotDeleteMountPoints()));
  _actDeleteMountPoints->setEnabled(false);

  _actGetData = new QAction(tr("Sta&rt"),this);
  connect(_actGetData, SIGNAL(triggered()), SLOT(slotGetData()));

  _actStop = new QAction(tr("Sto&p"),this);
  connect(_actStop, SIGNAL(triggered()), SLOT(slotStop()));
  _actStop->setEnabled(false);

  _actwhatsthis= new QAction(tr("Help=Shift+F1"),this);
  connect(_actwhatsthis, SIGNAL(triggered()), SLOT(slotWhatsThis()));

  CreateMenu();
  AddToolbar();

  bncSettings settings;
  _proxyHostLineEdit  = new QLineEdit(settings.value("proxyHost").toString());
  _proxyPortLineEdit  = new QLineEdit(settings.value("proxyPort").toString());
  _proxyPortLineEdit->setMaximumWidth(9*ww);
  _miscMountLineEdit  = new QLineEdit(settings.value("miscMount").toString());
  _scanRTCMCheckBox  = new QCheckBox();
  _scanRTCMCheckBox->setCheckState(Qt::CheckState(
                                    settings.value("scanRTCM").toInt()));
  _waitTimeSpinBox   = new QSpinBox();
  _waitTimeSpinBox->setMinimum(1);
  _waitTimeSpinBox->setMaximum(30);
  _waitTimeSpinBox->setSingleStep(1);
  _waitTimeSpinBox->setSuffix(" sec");
  _waitTimeSpinBox->setMaximumWidth(9*ww);
  _waitTimeSpinBox->setValue(settings.value("waitTime").toInt());
  _outFileLineEdit    = new QLineEdit(settings.value("outFile").toString());
  _outPortLineEdit    = new QLineEdit(settings.value("outPort").toString());
  _outPortLineEdit->setMaximumWidth(9*ww);
  _outUPortLineEdit   = new QLineEdit(settings.value("outUPort").toString());
  _outUPortLineEdit->setMaximumWidth(9*ww);
  _outEphPortLineEdit    = new QLineEdit(settings.value("outEphPort").toString());
  _outEphPortLineEdit->setMaximumWidth(9*ww);
  _corrPortLineEdit    = new QLineEdit(settings.value("corrPort").toString());
  _corrPortLineEdit->setMaximumWidth(9*ww);
  _rnxPathLineEdit    = new QLineEdit(settings.value("rnxPath").toString());
  _ephPathLineEdit    = new QLineEdit(settings.value("ephPath").toString());
  _corrPathLineEdit    = new QLineEdit(settings.value("corrPath").toString());

  _rnxV3CheckBox = new QCheckBox();
  _rnxV3CheckBox->setCheckState(Qt::CheckState(settings.value("rnxV3").toInt()));
  _ephV3CheckBox = new QCheckBox();
  _ephV3CheckBox->setCheckState(Qt::CheckState(settings.value("ephV3").toInt()));
  _rnxScrpLineEdit    = new QLineEdit(settings.value("rnxScript").toString());
  _rnxSkelLineEdit    = new QLineEdit(settings.value("rnxSkel").toString());
  _rnxSkelLineEdit->setMaximumWidth(5*ww);
  _rnxAppendCheckBox  = new QCheckBox();
  _rnxAppendCheckBox->setCheckState(Qt::CheckState(
                                    settings.value("rnxAppend").toInt()));
  _autoStartCheckBox  = new QCheckBox();
  _autoStartCheckBox->setCheckState(Qt::CheckState(
                                    settings.value("autoStart").toInt()));
  _rnxIntrComboBox    = new QComboBox();
  _rnxIntrComboBox->setMaximumWidth(9*ww);
  _rnxIntrComboBox->setEditable(false);
  _rnxIntrComboBox->addItems(QString("1 min,2 min,5 min,10 min,15 min,30 min,1 hour,1 day").split(","));
  int ii = _rnxIntrComboBox->findText(settings.value("rnxIntr").toString());
  if (ii != -1) {
    _rnxIntrComboBox->setCurrentIndex(ii);
  }
  _onTheFlyComboBox = new QComboBox();
  _onTheFlyComboBox->setMaximumWidth(9*ww);
  _onTheFlyComboBox->setEditable(false);
  _onTheFlyComboBox->addItems(QString("1 day,1 hour,1 min").split(","));
  ii = _onTheFlyComboBox->findText(settings.value("onTheFlyInterval").toString());
  if (ii != -1) {
    _onTheFlyComboBox->setCurrentIndex(ii);
  }
  _ephIntrComboBox    = new QComboBox();
  _ephIntrComboBox->setMaximumWidth(9*ww);
  _ephIntrComboBox->setEditable(false);
  _ephIntrComboBox->addItems(QString("1 min,2 min,5 min,10 min,15 min,30 min,1 hour,1 day").split(","));
  int jj = _ephIntrComboBox->findText(settings.value("ephIntr").toString());
  if (jj != -1) {
    _ephIntrComboBox->setCurrentIndex(jj);
  }
  _corrIntrComboBox    = new QComboBox();
  _corrIntrComboBox->setMaximumWidth(9*ww);
  _corrIntrComboBox->setEditable(false);
  _corrIntrComboBox->addItems(QString("1 min,2 min,5 min,10 min,15 min,30 min,1 hour,1 day").split(","));
  int mm = _corrIntrComboBox->findText(settings.value("corrIntr").toString());
  if (mm != -1) {
    _corrIntrComboBox->setCurrentIndex(mm);
  }
  _corrTimeSpinBox   = new QSpinBox();
  _corrTimeSpinBox->setMinimum(1);
  _corrTimeSpinBox->setMaximum(30);
  _corrTimeSpinBox->setSingleStep(1);
  _corrTimeSpinBox->setSuffix(" sec");
  _corrTimeSpinBox->setMaximumWidth(9*ww);
  _corrTimeSpinBox->setValue(settings.value("corrTime").toInt());
  _rnxSamplSpinBox    = new QSpinBox();
  _rnxSamplSpinBox->setMinimum(0);
  _rnxSamplSpinBox->setMaximum(60);
  _rnxSamplSpinBox->setSingleStep(5);
  _rnxSamplSpinBox->setMaximumWidth(9*ww);
  _rnxSamplSpinBox->setValue(settings.value("rnxSampl").toInt());
  _rnxSamplSpinBox->setSuffix(" sec");

  _binSamplSpinBox    = new QSpinBox();
  _binSamplSpinBox->setMinimum(0);
  _binSamplSpinBox->setMaximum(60);
  _binSamplSpinBox->setSingleStep(5);
  _binSamplSpinBox->setMaximumWidth(9*ww);
  _binSamplSpinBox->setValue(settings.value("binSampl").toInt());
  _binSamplSpinBox->setSuffix(" sec");

  _obsRateComboBox    = new QComboBox();
  _obsRateComboBox->setMaximumWidth(9*ww);
  _obsRateComboBox->setEditable(false);
  _obsRateComboBox->addItems(QString(",0.1 Hz,0.2 Hz,0.5 Hz,1 Hz,5 Hz").split(","));
  int kk = _obsRateComboBox->findText(settings.value("obsRate").toString());
  if (kk != -1) {
    _obsRateComboBox->setCurrentIndex(kk);
  }
  _adviseRecoSpinBox = new QSpinBox();
  _adviseRecoSpinBox->setMinimum(0);
  _adviseRecoSpinBox->setMaximum(60);
  _adviseRecoSpinBox->setSingleStep(1);
  _adviseRecoSpinBox->setSuffix(" min");
  _adviseRecoSpinBox->setMaximumWidth(9*ww);
  _adviseRecoSpinBox->setValue(settings.value("adviseReco").toInt());
  _adviseFailSpinBox = new QSpinBox();
  _adviseFailSpinBox->setMinimum(0);
  _adviseFailSpinBox->setMaximum(60);
  _adviseFailSpinBox->setSingleStep(1);
  _adviseFailSpinBox->setSuffix(" min");
  _adviseFailSpinBox->setMaximumWidth(9*ww);
  _adviseFailSpinBox->setValue(settings.value("adviseFail").toInt());
  _logFileLineEdit    = new QLineEdit(settings.value("logFile").toString());
  _adviseScriptLineEdit    = new QLineEdit(settings.value("adviseScript").toString());

  _serialPortNameLineEdit = new QLineEdit(settings.value("serialPortName").toString());
  _serialMountPointLineEdit = new QLineEdit(settings.value("serialMountPoint").toString());


  _serialBaudRateComboBox = new QComboBox();
  _serialBaudRateComboBox->setMaximumWidth(9*ww);
  _serialBaudRateComboBox->addItems(QString("110,300,600,"
            "1200,2400,4800,9600,19200,38400,57600,115200").split(","));
  kk = _serialBaudRateComboBox->findText(settings.value("serialBaudRate").toString());
  if (kk != -1) {
    _serialBaudRateComboBox->setCurrentIndex(kk);
  }
  _serialParityComboBox   = new QComboBox();
  _serialParityComboBox->setMaximumWidth(9*ww);
  _serialParityComboBox->addItems(QString("NONE,ODD,EVEN,SPACE").split(","));
  kk = _serialParityComboBox->findText(settings.value("serialParity").toString());
  if (kk != -1) {
    _serialParityComboBox->setCurrentIndex(kk);
  }
  _serialDataBitsComboBox = new QComboBox();
  _serialDataBitsComboBox->setMaximumWidth(5*ww);
  _serialDataBitsComboBox->addItems(QString("5,6,7,8").split(","));
  kk = _serialDataBitsComboBox->findText(settings.value("serialDataBits").toString());
  if (kk != -1) {
    _serialDataBitsComboBox->setCurrentIndex(kk);
  }
  _serialStopBitsComboBox = new QComboBox();
  _serialStopBitsComboBox->setMaximumWidth(5*ww);
  _serialStopBitsComboBox->addItems(QString("1,2").split(","));
  kk = _serialStopBitsComboBox->findText(settings.value("serialStopBits").toString());
  if (kk != -1) {
    _serialStopBitsComboBox->setCurrentIndex(kk);
  }
  _serialAutoNMEAComboBox  = new QComboBox();
  _serialAutoNMEAComboBox->setMaximumWidth(9*ww);
  _serialAutoNMEAComboBox->addItems(QString("Auto,Manual").split(","));
  kk = _serialAutoNMEAComboBox->findText(settings.value("serialAutoNMEA").toString());
  if (kk != -1) {
    _serialAutoNMEAComboBox->setCurrentIndex(kk);
  }
  _serialFileNMEALineEdit    = new QLineEdit(settings.value("serialFileNMEA").toString());
  _serialHeightNMEALineEdit  = new QLineEdit(settings.value("serialHeightNMEA").toString());
  _perfIntrComboBox    = new QComboBox();
  _perfIntrComboBox->setMaximumWidth(9*ww);
  _perfIntrComboBox->setEditable(false);
  _perfIntrComboBox->addItems(QString(",2 sec, 10 sec,1 min,5 min,15 min,1 hour,6 hours,1 day").split(","));
  int ll = _perfIntrComboBox->findText(settings.value("perfIntr").toString());
  if (ll != -1) {
    _perfIntrComboBox->setCurrentIndex(ll);
  }

  _mountPointsTable   = new QTableWidget(0,8);

  _mountPointsTable->horizontalHeader()->resizeSection(1,34*ww);
  _mountPointsTable->horizontalHeader()->resizeSection(2,9*ww);
  _mountPointsTable->horizontalHeader()->resizeSection(3,7*ww); 
  _mountPointsTable->horizontalHeader()->resizeSection(4,7*ww); 
  _mountPointsTable->horizontalHeader()->resizeSection(5,5*ww); 
  _mountPointsTable->horizontalHeader()->resizeSection(6,4*ww); 
  _mountPointsTable->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  _mountPointsTable->horizontalHeader()->setStretchLastSection(true);
  _mountPointsTable->setHorizontalHeaderLabels(labels);
  _mountPointsTable->setGridStyle(Qt::NoPen);
  _mountPointsTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  _mountPointsTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _mountPointsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  _mountPointsTable->hideColumn(0);
  connect(_mountPointsTable, SIGNAL(itemSelectionChanged()), 
          SLOT(slotSelectionChanged()));
  populateMountPointsTable();

  _log = new QTextBrowser();
  _log->setReadOnly(true);

  // WhatsThis
  // ---------
  _proxyHostLineEdit->setWhatsThis(tr("<p>If you are running BNC within a protected Local Area Network (LAN), you might need to use a proxy server to access the Internet. Enter your proxy server IP and port number in case one is operated in front of BNC. If you do not know the IP and port of your proxy server, check the proxy server settings in your Internet browser or ask your network administrator.</p><p>Note that IP streaming is sometimes not allowed in a LAN. In this case you need to ask your network administrator for an appropriate modification of the local security policy or for the installation of a TCP relay to the NTRIP broadcasters. If these are not possible, you might need to run BNC outside your LAN on a network that has unobstructed connection to the Internet.</p>"));
  _proxyPortLineEdit->setWhatsThis(tr("<p>Enter your proxy server port number in case a proxy is operated in front of BNC.</p>"));
  _waitTimeSpinBox->setWhatsThis(tr("<p>When feeding a real-time GNSS network engine waiting for synchronized input epoch by epoch, BNC drops whatever is received later than 'Wait for full epoch' seconds. A value of 3 to 5 seconds is recommended, depending on the latency of the incoming streams and the delay acceptable to your real-time GNSS network engine or products.</p>"));
  _outFileLineEdit->setWhatsThis(tr("Specify the full path to a file where synchronized observations are saved in plain ASCII format. Beware that the size of this file can rapidly increase depending on the number of incoming streams."));
  _outPortLineEdit->setWhatsThis(tr("BNC can produce synchronized observations in binary format on your local host through an IP port. Specify a port number here to activate this function."));
  _outUPortLineEdit->setWhatsThis(tr("BNC can produce unsynchronized observations in binary format on your local host through an IP port. Specify a port number here to activate this function."));
  _outEphPortLineEdit->setWhatsThis(tr("BNC can produce ephemeris data in RINEX ASCII format on your local host through an IP port. Specify a port number here to activate this function."));
  _corrPortLineEdit->setWhatsThis(tr("BNC can produce Broadcast Ephemeris Corrections on your local host through an IP port. Specify a port number here to activate this function."));
  _corrTimeSpinBox->setWhatsThis(tr("Concerning output through IP port, BNC drops Broadcast Ephemeris Corrections received later than 'Wait for full epoch' seconds. A value of 2 to 5 seconds is recommended, depending on the latency of the incoming correction stream(s) and the delay acceptable to your real-time application."));
  _rnxPathLineEdit->setWhatsThis(tr("Here you specify the path to where the RINEX Observation files will be stored. If the specified directory does not exist, BNC will not create RINEX Observation files.")); 
  _ephPathLineEdit->setWhatsThis(tr("Specify the path for saving Broadcast Ephemeris data as RINEX Navigation files. If the specified directory does not exist, BNC will not create RINEX Navigation files."));
  _corrPathLineEdit->setWhatsThis(tr("Specify a directory for saving Broadcast Ephemeris Correction files. If the specified directory does not exist, BNC will not create the files."));
  _rnxScrpLineEdit->setWhatsThis(tr("<p>Whenever a RINEX Observation file is saved, you might want to compress, copy or upload it immediately via FTP. BNC allows you to execute a script/batch file to carry out these operations. To do that specify the full path of the script/batch file here. BNC will pass the full RINEX Observation file path to the script as a command line parameter (%1 on Windows systems, $1 onUnix/Linux systems).</p>"));
  _rnxSkelLineEdit->setWhatsThis(tr("<p>BNC allows using personal skeleton files that contain the header records you would like to include. You can derive a personal RINEX header skeleton file from the information given in an up to date sitelog.</p><p>A file in the RINEX Observations 'Directory' with a 'Skeleton extension' suffix is interpreted by BNC as a personal RINEX header skeleton file for the corresponding stream.</p>"));
  _rnxAppendCheckBox->setWhatsThis(tr("<p>When BNC is started, new files are created by default and any existing files with the same name will be overwritten. However, users might want to append already existing files following a restart of BNC, a system crash or when BNC crashed. Tick 'Append files' to continue with existing files and keep what has been recorded so far.</p>"));
  _autoStartCheckBox->setWhatsThis(tr("<p>Tick 'Auto start' for auto-start of BNC at startup time in window mode with preassigned processing options.</p>"));
  _onTheFlyComboBox->setWhatsThis(tr("<p>When operating BNC online in 'no window' mode, some configuration parameters can be changed on-the-fly without interrupting the running process. For that BNC rereads parts of its configuration in pre-defined intervals.<p></p>Select '1 min', '1 hour', or '1 day' to force BNC to reread its configuration every full minute, hour, or day and let in between edited configuration options become effective on-the-fly without terminating uninvolved threads.</p><p>Note that when operating BNC in window mode, on-the-fly changeable configuration options become effective immediately through 'Save & Activate Options'.</p>"));
  _rnxIntrComboBox->setWhatsThis(tr("<p>Select the length of the RINEX Observation file.</p>"));
  _ephIntrComboBox->setWhatsThis(tr("<p>Select the length of the RINEX Navigation file.</p>"));
  _corrIntrComboBox->setWhatsThis(tr("<p>Select the length of the Broadcast Ephemeris Correction files.</p>"));
  _rnxSamplSpinBox->setWhatsThis(tr("<p>Select the RINEX Observation sampling interval in seconds. A value of zero '0' tells BNC to store all received epochs into RINEX.</p>"));
  _binSamplSpinBox->setWhatsThis(tr("<p>Select the synchronized observation sampling interval in seconds. A value of zero '0' tells BNC to send/store all received epochs.</p>"));
  _obsRateComboBox->setWhatsThis(tr("<p>BNC can collect all returns (success or failure) coming from a decoder within a certain short time span to then decide whether a stream has an outage or its content is corrupted. The procedure needs a rough estimate of the expected 'Observation rate' of the incoming streams. When a continuous problem is detected, BNC can inform its operator about this event through an advisory note.</p>"));
  _adviseRecoSpinBox->setWhatsThis(tr("<p>Following a stream outage or a longer series of bad observations, an advisory note is generated when valid observations are received again throughout the 'Recovery threshold' time span. A value of about 5min (default) is recommended.</p><p>A value of zero '0' means that for any stream recovery, however short, BNC immediately generates an advisory note.</p>"));
  _adviseFailSpinBox->setWhatsThis(tr("<p>An advisory note is generated when no (or only corrupted) observations are seen throughout the 'Failure threshold' time span. A value of 15 min (default) is recommended.</p><p>A value of zero '0' means that for any stream failure, however short, BNC immediately generates an advisory note.</p>"));
  _logFileLineEdit->setWhatsThis(tr("<p>Records of BNC's activities are shown in the 'Logs' section on the bottom of this window. They can be saved into a file when a valid path is specified in the 'Logfile (full path)' field.</p><p>The logfile name will automatically be extended by a string '_YYMMDD' carrying the current date."));
  _adviseScriptLineEdit->setWhatsThis(tr("<p>Specify the full path to a script or batch file to handle advisory notes generated in the event of corrupted streams or stream outages. The affected mountpoint and one of the comments 'Begin_Outage', 'End_Outage', 'Begin_Corrupted', or 'End_Corrupted' are passed on to the script as command line parameters.</p><p>The script can be configured to send an email to BNC's operator and/or to the affected stream provider. An empty option field (default) or invalid path means that you don't want to use this option.</p>"));
  _perfIntrComboBox->setWhatsThis(tr("<p>BNC can average latencies per stream over a certain period of GPS time. The resulting mean latencies are recorded in the 'Logs' section at the end of each 'Log latency' interval together with results of a statistical evaluation (approximate number of covered epochs, data gaps).</p><p>Select a 'Log latency' interval or select the empty option field if you do not want BNC to log latencies and statistical information.</p>"));
  _mountPointsTable->setWhatsThis(tr("<p>Streams selected for retrieval are listed in the 'Streams' section. Clicking on 'Add Streams' button will open a window that allows the user to select data streams from an NTRIP broadcaster according to their mountpoints. To remove a stream from the 'Streams' list, highlight it by clicking on it and hit the 'Delete Streams' button. You can also remove multiple streams by highlighting them using +Shift and +Ctrl.</p><p>BNC automatically allocates one of its internal decoders to a stream based on the stream's 'format' as given in the sourcetable. BNC allows users to change this selection by editing the decoder string. Double click on the 'decoder' field, enter your preferred decoder and then hit Enter. The accepted decoder strings are 'RTCM_2.x', 'RTCM_3.x', and 'RTIGS'.</p><p>In case you need to log the raw data as is, BNC allows users to by-pass its decoders and and directly save the input in daily log files. To do this specify the decoder string as 'ZERO'.</p><p>BNC can also retrieve streams from virtual reference stations (VRS). VRS streams are indicated by a 'yes' in the 'nmea' column. To initiate these streams, the approximate latitude/longitude rover position is sent to the NTRIP broadcaster. The default values can be change according to your requirement. Double click on 'lat' and 'long' fields, enter the values you wish to send and then hit Enter.</p>"));
  _log->setWhatsThis(tr("Records of BNC's activities are shown in the 'Logs' section. The message log covers the communication status between BNC and the NTRIP broadcaster as well as any problems that occur in the communication link, stream availability, stream delay, stream conversion etc."));
  _ephV3CheckBox->setWhatsThis(tr("The default format for output of RINEX Navigation data containing Broadcast Ephemeris is RINEX Version 2.11. Select 'Version 3' if you want to output the ephemeris in RINEX Version 3 format."));
  _rnxV3CheckBox->setWhatsThis(tr("The default format for RINEX Observation files is RINEX Version 2.11. Select 'Version 3' if you want to save the observations in RINEX Version 3 format."));
  _miscMountLineEdit->setWhatsThis(tr("<p>Specify a mountpoint to apply any of the options shown below. Enter 'ALL' if you want to apply these options to all configured streams.</p><p>An empty option field (default) means that you don't want BNC to apply any of these options.</p>"));
  _scanRTCMCheckBox->setWhatsThis(tr("<p>Tick 'Scan RTCM' to log the numbers of incomming message types as well as contained antenna coordinates, antenna heigt, and antenna descriptor.</p>"));
  _serialMountPointLineEdit->setWhatsThis(tr("<p>Enter a 'Mountpoint' to forward the corresponding stream to a serial connected device.</p>"));
  _serialPortNameLineEdit->setWhatsThis(tr("<p>Enter the serial 'Port name' selected for communication with your serial connected device. Valid port names are</p><pre>Windows:       COM1, COM2<br>Linux:         /dev/ttyS0, /dev/ttyS1<br>FreeBSD:       /dev/ttyd0, /dev/ttyd1<br>Digital Unix:  /dev/tty01, /dev/tty02<br>HP-UX:         /dev/tty1p0, /dev/tty2p0<br>SGI/IRIX:      /dev/ttyf1, /dev/ttyf2<br>SunOS/Solaris: /dev/ttya, /dev/ttyb</pre><p>Note that you must plug a serial cable in the port defined here before you start BNC.</p>"));
  _serialBaudRateComboBox->setWhatsThis(tr("<p>Select a 'Baud rate' for the serial link.</p><p>Note that your selection must equal the baud rate configured to the serial connected device. Note further that using a high baud rate is recommended.</p>"));
  _serialParityComboBox->setWhatsThis(tr("<p>Select the 'Parity' for the serial link.</p><p>Note that your selection must equal the parity selection configured to the serial connected device. Note further that parity is often set to 'NONE'.</p>"));
  _serialDataBitsComboBox->setWhatsThis(tr("<p>Select the number of 'Data bits' for the serial link.</p><p>Note that your selection must equal the number of data bits configured to the serial connected device. Note further that often 8 data bits are used.</p>"));
  _serialStopBitsComboBox->setWhatsThis(tr("<p>Select the number of 'Stop bits' for the serial link.</p><p>Note that your selection must equal the number of stop bits configured to the serial connected device. Note further that often 1 stop bit is used.</p>"));
  _serialAutoNMEAComboBox->setWhatsThis(tr("<p>Concerning virtual reference stations (VRS):<p></p>Select 'Auto' to automatically forward NMEA GGA messages coming from your serial connected device to the NTRIP broadcaster.</p><p>The alternative is a 'Manual' simulation of an initial NMEA GGA message based on the approximate (editable) VRS latitude/longitude from the broadcaster's sourcetable and an approximate VRS height to be specified.</p><p>The setting of this option is ignored in case of streams coming from physical reference stations.</p>"));
  _serialFileNMEALineEdit->setWhatsThis(tr("<p>Concerning virtual reference stations (VRS):</p><p>Specify the full path to a file where NMEA messages coming from your serial connected device are saved.</p><p>The setting of this option is ignored in case of streams coming from physical reference stations.</p>"));
  _serialHeightNMEALineEdit->setWhatsThis(tr("<p>Concerning virtual reference stations (VRS):<p></p>Specify an approximate 'Height' above mean sea level in meter for your VRS to simulate an inital NMEA GGA message.</p><p>The setting of this option is ignored in case of streams coming from physical reference stations.</p>"));

  // Canvas with Editable Fields
  // ---------------------------
  _canvas = new QWidget;
  setCentralWidget(_canvas);

//QTabWidget* aogroup = new QTabWidget();
  aogroup = new QTabWidget();
  QWidget* pgroup = new QWidget();
  QWidget* ggroup = new QWidget();
  QWidget* sgroup = new QWidget();
  QWidget* egroup = new QWidget();
  QWidget* agroup = new QWidget();
  QWidget* cgroup = new QWidget();
  QWidget* ogroup = new QWidget();
  QWidget* rgroup = new QWidget();
  QWidget* sergroup = new QWidget();
  aogroup->addTab(pgroup,tr("Proxy"));
  aogroup->addTab(ggroup,tr("General"));
  aogroup->addTab(ogroup,tr("RINEX Observations"));
  aogroup->addTab(egroup,tr("RINEX Ephemeris"));
  aogroup->addTab(cgroup,tr("Ephemeris Corrections"));
  aogroup->addTab(sgroup,tr("Feed Engine"));
  aogroup->addTab(sergroup,tr("Serial Link"));
  aogroup->addTab(agroup,tr("Outages"));
  aogroup->addTab(rgroup,tr("Miscellaneous"));

  // Proxy
  // ----
  QGridLayout* pLayout = new QGridLayout;
  pLayout->setColumnMinimumWidth(0,13*ww);
  pLayout->addWidget(new QLabel("Proxy host"),                   0, 0);
  pLayout->addWidget(_proxyHostLineEdit,                         0, 1, 1,10);
  pLayout->addWidget(new QLabel("Proxy port"),                   1, 0);
  pLayout->addWidget(_proxyPortLineEdit,                         1, 1);
  pLayout->addWidget(new QLabel("Settings for the proxy in protected networks, leave boxes blank if none."),2, 0, 1, 50, Qt::AlignLeft);
  pLayout->addWidget(new QLabel("    "),3,0);
  pLayout->addWidget(new QLabel("    "),4,0);
  pLayout->addWidget(new QLabel("    "),5,0);
  pgroup->setLayout(pLayout);


  connect(_proxyHostLineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(bncText(const QString &)));
  if (_proxyHostLineEdit->text().isEmpty()) { 
    _proxyPortLineEdit->setStyleSheet("background-color: lightGray");
    _proxyPortLineEdit->setEnabled(false);
  }
 
  // General
  // -------
  QGridLayout* gLayout = new QGridLayout;
  gLayout->setColumnMinimumWidth(0,14*ww);
  gLayout->addWidget(new QLabel("Logfile (full path)"),          0, 0);
  gLayout->addWidget(_logFileLineEdit,                           0, 1);
  gLayout->addWidget(new QLabel("Append files"),                 1, 0);
  gLayout->addWidget(_rnxAppendCheckBox,                         1, 1);
  gLayout->addWidget(new QLabel("Reread configuration"),         2, 0);
  gLayout->addWidget(_onTheFlyComboBox,                          2, 1);
  gLayout->addWidget(new QLabel("Auto start"),                   3, 0);
  gLayout->addWidget(_autoStartCheckBox,                         3, 1);
  gLayout->addWidget(new QLabel("General settings for logfile, file handling, configuration on-the-fly, and auto-start."),4, 0, 1, 2, Qt::AlignLeft);
  gLayout->addWidget(new QLabel("    "),5,0);
  ggroup->setLayout(gLayout);

  // Feed Engine
  // -----------
  QGridLayout* sLayout = new QGridLayout;
  sLayout->setColumnMinimumWidth(0,14*ww);
  sLayout->addWidget(new QLabel("Port"),                          0, 0);
  sLayout->addWidget(_outPortLineEdit,                            0, 1);
  sLayout->addWidget(new QLabel("Wait for full epoch"),           0, 2, Qt::AlignRight);
  sLayout->addWidget(_waitTimeSpinBox,                            0, 3, Qt::AlignLeft);
  sLayout->addWidget(new QLabel("Sampling"),                      1, 0);
  sLayout->addWidget(_binSamplSpinBox,                            1, 1, Qt::AlignLeft);
  sLayout->addWidget(new QLabel("File (full path)"),              2, 0);
  sLayout->addWidget(_outFileLineEdit,                            2, 1, 1, 30);
  sLayout->addWidget(new QLabel("Port (unsynchronized)"),         3, 0);
  sLayout->addWidget(_outUPortLineEdit,                           3, 1);
  sLayout->addWidget(new QLabel("Output decoded observations in a binary format to feed a real-time GNSS engine."),4,0,1,30);
  sLayout->addWidget(new QLabel("    "),5,0);
  sgroup->setLayout(sLayout);

  connect(_outPortLineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(bncText(const QString &)));
  connect(_outFileLineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(bncText(const QString &)));
  if (_outPortLineEdit->text().isEmpty() && _outFileLineEdit->text().isEmpty()) { 
    _waitTimeSpinBox->setStyleSheet("background-color: lightGray");
    _binSamplSpinBox->setStyleSheet("background-color: lightGray");
    _waitTimeSpinBox->setEnabled(false);
    _binSamplSpinBox->setEnabled(false);
  }

  // RINEX Ephemeris
  // ---------------
  QGridLayout* eLayout = new QGridLayout;
  eLayout->setColumnMinimumWidth(0,14*ww);
  eLayout->addWidget(new QLabel("Directory"),                     0, 0);
  eLayout->addWidget(_ephPathLineEdit,                            0, 1);
  eLayout->addWidget(new QLabel("Interval"),                      1, 0);
  eLayout->addWidget(_ephIntrComboBox,                            1, 1);
  eLayout->addWidget(new QLabel("Port"),                          2, 0);
  eLayout->addWidget(_outEphPortLineEdit,                         2, 1);
  eLayout->addWidget(new QLabel("Version 3"),                     3, 0);
  eLayout->addWidget(_ephV3CheckBox,                              3, 1);
  eLayout->addWidget(new QLabel("Saving RINEX ephemeris files and ephemeris output through IP port."),4,0,1,2,Qt::AlignLeft);
  eLayout->addWidget(new QLabel("    "),5,0);
  egroup->setLayout(eLayout);

  connect(_ephPathLineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(bncText(const QString &)));
  connect(_outEphPortLineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(bncText(const QString &)));

  if (_ephPathLineEdit->text().isEmpty() && _outEphPortLineEdit->text().isEmpty()) { 
    _ephIntrComboBox->setStyleSheet("background-color: lightGray");
    palette.setColor(_ephV3CheckBox->backgroundRole(), lightGray);
    _ephV3CheckBox->setPalette(palette);
    _ephIntrComboBox->setEnabled(false);
    _ephV3CheckBox->setEnabled(false);
  }
  if (_ephPathLineEdit->text().isEmpty() && !_outEphPortLineEdit->text().isEmpty()) { 
    _ephIntrComboBox->setStyleSheet("background-color: lightGray");
    _ephIntrComboBox->setEnabled(false);
  }   

  // Outages
  // -------
  QGridLayout* aLayout = new QGridLayout;
  aLayout->setColumnMinimumWidth(0,14*ww);
  aLayout->addWidget(new QLabel("Observation rate"),              0, 0);
  aLayout->addWidget(_obsRateComboBox,                            0, 1);
  aLayout->addWidget(new QLabel("Failure threshold"),             1, 0);
  aLayout->addWidget(_adviseFailSpinBox,                          1, 1);
  aLayout->addWidget(new QLabel("Recovery threshold"),            2, 0);
  aLayout->addWidget(_adviseRecoSpinBox,                          2, 1);
  aLayout->addWidget(new QLabel("Script (full path)"),            3, 0);
  aLayout->addWidget(_adviseScriptLineEdit,                       3, 1,1,10);
  aLayout->addWidget(new QLabel("Outage report, handling of corrupted streams."),5,0,1,10,Qt::AlignLeft);
  agroup->setLayout(aLayout);

  connect(_obsRateComboBox, SIGNAL(currentIndexChanged(const QString &)),
          this, SLOT(bncText(const QString)));
  if (_obsRateComboBox->currentText().isEmpty()) { 
    _adviseFailSpinBox->setStyleSheet("background-color: lightGray");
    _adviseRecoSpinBox->setStyleSheet("background-color: lightGray");
    _adviseScriptLineEdit->setStyleSheet("background-color: lightGray");
    _adviseFailSpinBox->setEnabled(false);
    _adviseRecoSpinBox->setEnabled(false);
    _adviseScriptLineEdit->setEnabled(false);
  }

  // Miscellaneous
  // -------------
  QGridLayout* rLayout = new QGridLayout;
  rLayout->setColumnMinimumWidth(0,14*ww);
  rLayout->addWidget(new QLabel("Mountpoint"),                    0, 0);
  rLayout->addWidget(_miscMountLineEdit,                          0, 1, 1,7);
  rLayout->addWidget(new QLabel("Log latency"),                   1, 0);
  rLayout->addWidget(_perfIntrComboBox,                           1, 1);
  rLayout->addWidget(new QLabel("Scan RTCM"),                     2, 0);
  rLayout->addWidget(_scanRTCMCheckBox,                           2, 1);
  rLayout->addWidget(new QLabel("Log latencies or scan RTCM streams for numbers of message types and antenna information."),3, 0,1,30);
  rLayout->addWidget(new QLabel("    "),                          4, 0);
  rLayout->addWidget(new QLabel("    "),                          5, 0);
  rgroup->setLayout(rLayout);

  connect(_miscMountLineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(bncText(const QString &)));
  if (_miscMountLineEdit->text().isEmpty()) { 
    _perfIntrComboBox->setStyleSheet("background-color: lightGray");
    palette.setColor(_scanRTCMCheckBox->backgroundRole(), lightGray);
    _scanRTCMCheckBox->setPalette(palette);
    _perfIntrComboBox->setEnabled(false);
    _scanRTCMCheckBox->setEnabled(false);
  }

  // RINEX Observations
  // ------------------
  QGridLayout* oLayout = new QGridLayout;
  oLayout->setColumnMinimumWidth(0,14*ww);
  oLayout->addWidget(new QLabel("Directory"),                     0, 0);
  oLayout->addWidget(_rnxPathLineEdit,                            0, 1,1,12);
  oLayout->addWidget(new QLabel("Interval"),                      1, 0);
  oLayout->addWidget(_rnxIntrComboBox,                            1, 1);
  oLayout->addWidget(new QLabel("Sampling"),                      1, 2, Qt::AlignRight);
  oLayout->addWidget(_rnxSamplSpinBox,                            1, 3, Qt::AlignLeft);
  oLayout->addWidget(new QLabel("Skeleton extension"),            2, 0);
  oLayout->addWidget(_rnxSkelLineEdit,                            2, 1,1,1, Qt::AlignLeft);
  oLayout->addWidget(new QLabel("Script (full path)"),            3, 0);
  oLayout->addWidget(_rnxScrpLineEdit,                            3, 1,1,12);
  oLayout->addWidget(new QLabel("Version 3"),                     4, 0);
  oLayout->addWidget(_rnxV3CheckBox,                              4, 1);
  oLayout->addWidget(new QLabel("Saving RINEX observation files."),5,0,1,12, Qt::AlignLeft);
  ogroup->setLayout(oLayout);

  connect(_rnxPathLineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(bncText(const QString &)));
  if (_rnxPathLineEdit->text().isEmpty()) { 
    _rnxIntrComboBox->setStyleSheet("background-color: lightGray");
    _rnxSamplSpinBox->setStyleSheet("background-color: lightGray");
    _rnxSkelLineEdit->setStyleSheet("background-color: lightGray");
    _rnxScrpLineEdit->setStyleSheet("background-color: lightGray");
    palette.setColor(_rnxV3CheckBox->backgroundRole(), lightGray);
    _rnxV3CheckBox->setPalette(palette);
    _rnxIntrComboBox->setEnabled(false);
    _rnxSamplSpinBox->setEnabled(false);
    _rnxSkelLineEdit->setEnabled(false);
    _rnxScrpLineEdit->setEnabled(false);
    _rnxV3CheckBox->setEnabled(false);
  }

  // Ephemeris Corrections
  // ---------------------
  QGridLayout* cLayout = new QGridLayout;
  cLayout->setColumnMinimumWidth(0,14*ww);
  cLayout->addWidget(new QLabel("Directory"),                     0, 0);
  cLayout->addWidget(_corrPathLineEdit,                           0, 1,1,30);
  cLayout->addWidget(new QLabel("Interval"),                      1, 0);
  cLayout->addWidget(_corrIntrComboBox,                           1, 1);
  cLayout->addWidget(new QLabel("Port"),                          2, 0);
  cLayout->addWidget(_corrPortLineEdit,                           2, 1);
  cLayout->addWidget(new QLabel("Wait for full epoch"),           2, 2, Qt::AlignRight);
  cLayout->addWidget(_corrTimeSpinBox,                            2, 3, Qt::AlignLeft);
  cLayout->addWidget(new QLabel("Saving Broadcast Ephemeris correction files and correction output through IP port."),3,0,1,30);
  cLayout->addWidget(new QLabel("    "),4,0);
  cLayout->addWidget(new QLabel("    "),5,0);
  cgroup->setLayout(cLayout);

  connect(_corrPathLineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(bncText(const QString &)));
  connect(_corrPortLineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(bncText(const QString &)));
  if (_corrPathLineEdit->text().isEmpty()) { 
    _corrIntrComboBox->setStyleSheet("background-color: lightGray");
    _corrIntrComboBox->setEnabled(false);
  }
  if (_corrPortLineEdit->text().isEmpty()) { 
    _corrTimeSpinBox->setStyleSheet("background-color: lightGray");
    _corrTimeSpinBox->setEnabled(false);
  }

  // Serial Link
  // -----------
  QGridLayout* serLayout = new QGridLayout;
  serLayout->setColumnMinimumWidth(0,14*ww);
  _serialHeightNMEALineEdit->setMaximumWidth(8*ww);
  serLayout->addWidget(new QLabel("Mountpoint"),                  0,0, Qt::AlignLeft);
  serLayout->addWidget(_serialMountPointLineEdit,                 0,1,1,2);
  serLayout->addWidget(new QLabel("Port name"),                   1,0, Qt::AlignLeft);
  serLayout->addWidget(_serialPortNameLineEdit,                   1,1,1,2);
  serLayout->addWidget(new QLabel("Baud rate"),                   2,0, Qt::AlignLeft);
  serLayout->addWidget(_serialBaudRateComboBox,                   2,1);
  serLayout->addWidget(new QLabel("               Parity  "),     2,2, Qt::AlignRight);
  serLayout->addWidget(_serialParityComboBox,                     2,3);
  serLayout->addWidget(new QLabel("Data bits"),                   3,0, Qt::AlignLeft);
  serLayout->addWidget(_serialDataBitsComboBox,                   3,1);
  serLayout->addWidget(new QLabel("               Stop bits  "),  3,2, Qt::AlignRight);
  serLayout->addWidget(_serialStopBitsComboBox,                   3,3);
  serLayout->addWidget(new QLabel("VRS/NMEA"),                    4,0);
  serLayout->addWidget(_serialAutoNMEAComboBox,                   4,1);
  serLayout->addWidget(new QLabel("File (full path)"),            4,2, Qt::AlignRight);
  serLayout->addWidget(_serialFileNMEALineEdit,                   4,3,1,25);
  serLayout->addWidget(new QLabel("Height"),                      4,29, Qt::AlignRight);
  serLayout->addWidget(_serialHeightNMEALineEdit,                 4,30);
  serLayout->addWidget(new QLabel("Serial port settings to feed a serial connected device."),5,0,1,30);

  connect(_serialMountPointLineEdit, SIGNAL(textChanged(const QString &)),
          this, SLOT(bncText(const QString &)));
  connect(_serialAutoNMEAComboBox, SIGNAL(currentIndexChanged(const QString &)),
          this, SLOT(bncText(const QString)));
  if (_serialMountPointLineEdit->text().isEmpty()) { 
    _serialPortNameLineEdit->setStyleSheet("background-color: lightGray");
    _serialBaudRateComboBox->setStyleSheet("background-color: lightGray");
    _serialParityComboBox->setStyleSheet("background-color: lightGray");
    _serialDataBitsComboBox->setStyleSheet("background-color: lightGray");
    _serialStopBitsComboBox->setStyleSheet("background-color: lightGray");
    _serialAutoNMEAComboBox->setStyleSheet("background-color: lightGray");
    _serialFileNMEALineEdit->setStyleSheet("background-color: lightGray");
    _serialHeightNMEALineEdit->setStyleSheet("background-color: lightGray");
    _serialPortNameLineEdit->setEnabled(false);
    _serialBaudRateComboBox->setEnabled(false);
    _serialParityComboBox->setEnabled(false);
    _serialDataBitsComboBox->setEnabled(false);
    _serialStopBitsComboBox->setEnabled(false);
    _serialAutoNMEAComboBox->setEnabled(false);
    _serialFileNMEALineEdit->setEnabled(false);
    _serialHeightNMEALineEdit->setEnabled(false);
  } else {
    if (_serialAutoNMEAComboBox->currentText() == "Auto" ) {
      _serialHeightNMEALineEdit->setStyleSheet("background-color: lightGray");
      _serialHeightNMEALineEdit->setEnabled(false);
    } 
    if (_serialAutoNMEAComboBox->currentText() != "Auto" ) {
      _serialFileNMEALineEdit->setStyleSheet("background-color: lightGray");
      _serialFileNMEALineEdit->setEnabled(false);
    } 
  }

  sergroup->setLayout(serLayout);

  QGridLayout* mLayout = new QGridLayout;
  aogroup->setCurrentIndex(settings.value("startTab").toInt());
  mLayout->addWidget(aogroup,             0,0);
  mLayout->addWidget(_mountPointsTable,   1,0);
  mLayout->addWidget(new QLabel(" Logs:"),2,0);
  mLayout->addWidget(_log,                3,0);

  _canvas->setLayout(mLayout);

  // Auto start
  // ----------
  if ( Qt::CheckState(settings.value("autoStart").toInt()) == Qt::Checked) {
    slotGetData();
  }
}

// Destructor
////////////////////////////////////////////////////////////////////////////
bncWindow::~bncWindow() {
  delete _caster;
}

// 
////////////////////////////////////////////////////////////////////////////
void bncWindow::populateMountPointsTable() {

  for (int iRow = _mountPointsTable->rowCount()-1; iRow >=0; iRow--) {
    _mountPointsTable->removeRow(iRow);
  }

  bncSettings settings;

  QListIterator<QString> it(settings.value("mountPoints").toStringList());
  if (!it.hasNext()) {
    _actGetData->setEnabled(false);
  }
  int iRow = 0;
  while (it.hasNext()) {
    QStringList hlp = it.next().split(" ");
    if (hlp.size() < 5) continue;
    _mountPointsTable->insertRow(iRow);

    QUrl    url(hlp[0]);

    QString fullPath = url.host() + QString(":%1").arg(url.port()) + url.path();
    QString format(hlp[1]); QString latitude(hlp[2]); QString longitude(hlp[3]);
    QString nmea(hlp[4]);
    QString ntripVersion = "1";
    if (hlp.size() >= 6) {
      ntripVersion = (hlp[5]);
    }

    QTableWidgetItem* it;
    it = new QTableWidgetItem(url.userInfo());
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 0, it);

    it = new QTableWidgetItem(fullPath);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 1, it);

    it = new QTableWidgetItem(format);
    _mountPointsTable->setItem(iRow, 2, it);

    if      (nmea == "yes") {
    it = new QTableWidgetItem(latitude);
    _mountPointsTable->setItem(iRow, 3, it);
    it = new QTableWidgetItem(longitude);
    _mountPointsTable->setItem(iRow, 4, it);
    } else {
    it = new QTableWidgetItem(latitude);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 3, it);
    it = new QTableWidgetItem(longitude);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 4, it);
    }

    it = new QTableWidgetItem(nmea);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 5, it);

    it = new QTableWidgetItem(ntripVersion);
    ////    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 6, it);

    bncTableItem* bncIt = new bncTableItem();
    bncIt->setFlags(bncIt->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 7, bncIt);

    iRow++;
  }

  _mountPointsTable->sortItems(1);
}

// Retrieve Table
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotAddMountPoints() {

  bncSettings settings;
  QString proxyHost = settings.value("proxyHost").toString();
  int     proxyPort = settings.value("proxyPort").toInt();
  if (proxyHost != _proxyHostLineEdit->text()         ||
      proxyPort != _proxyPortLineEdit->text().toInt()) {
    int iRet = QMessageBox::question(this, "Question", "Proxy options "
                                     "changed. Use the new ones?", 
                                     QMessageBox::Yes, QMessageBox::No,
                                     QMessageBox::NoButton);
    if      (iRet == QMessageBox::Yes) {
      settings.setValue("proxyHost",   _proxyHostLineEdit->text());
      settings.setValue("proxyPort",   _proxyPortLineEdit->text());
      settings.sync();
    }
  }

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setWindowTitle("Add Streams");
  msgBox.setText("Add stream(s) coming from:");

  QPushButton *buttonNtrip = msgBox.addButton(tr("Caster"), QMessageBox::ActionRole);
  QPushButton *buttonIP = msgBox.addButton(tr("IP port"), QMessageBox::ActionRole);
  QPushButton *buttonCancel = msgBox.addButton(tr("Cancel"), QMessageBox::ActionRole);

  msgBox.exec();

  if (msgBox.clickedButton() == buttonNtrip) {
    bncTableDlg* dlg = new bncTableDlg(this); 
    dlg->move(this->pos().x()+50, this->pos().y()+50);
    connect(dlg, SIGNAL(newMountPoints(QStringList*)), 
          this, SLOT(slotNewMountPoints(QStringList*)));
    dlg->exec();
    delete dlg;
  } else if (msgBox.clickedButton() == buttonIP) {
    bncIpPort* ipp = new bncIpPort(this); 
    ipp->move(this->pos().x()+50, this->pos().y()+50);
    connect(ipp, SIGNAL(newMountPoints(QStringList*)), 
          this, SLOT(slotNewMountPoints(QStringList*)));
    ipp->exec();
    delete ipp;
  } else if (msgBox.clickedButton() == buttonCancel) {
    // Cancel
  }
}

// Delete Selected Mount Points
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotDeleteMountPoints() {

  int nRows = _mountPointsTable->rowCount();
  bool flg[nRows];
  for (int iRow = 0; iRow < nRows; iRow++) {
    if (_mountPointsTable->isItemSelected(_mountPointsTable->item(iRow,1))) {
      flg[iRow] = true;
    }
    else {
      flg[iRow] = false;
    }
  }
  for (int iRow = nRows-1; iRow >= 0; iRow--) {
    if (flg[iRow]) {
      _mountPointsTable->removeRow(iRow);
    }
  }
  _actDeleteMountPoints->setEnabled(false);

  if (_mountPointsTable->rowCount() == 0) {
    _actGetData->setEnabled(false);
  }
}

// New Mount Points Selected
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotNewMountPoints(QStringList* mountPoints) {
  int iRow = 0;
  QListIterator<QString> it(*mountPoints);
  while (it.hasNext()) {
    QStringList hlp = it.next().split(" ");
    QUrl    url(hlp[0]);
    QString fullPath = url.host() + QString(":%1").arg(url.port()) + url.path();
    QString format(hlp[1]); QString latitude(hlp[2]); QString longitude(hlp[3]);
    QString nmea(hlp[4]);
    QString ntripVersion = "1";
    if (hlp.size() >= 6) {
      ntripVersion = (hlp[5]);
    }

    _mountPointsTable->insertRow(iRow);

    QTableWidgetItem* it;
    it = new QTableWidgetItem(url.userInfo());
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 0, it);

    it = new QTableWidgetItem(fullPath);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 1, it);

    it = new QTableWidgetItem(format);
    _mountPointsTable->setItem(iRow, 2, it);

    if      (nmea == "yes") {
    it = new QTableWidgetItem(latitude);
    _mountPointsTable->setItem(iRow, 3, it);
    it = new QTableWidgetItem(longitude);
    _mountPointsTable->setItem(iRow, 4, it);
    } else {
    it = new QTableWidgetItem(latitude);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 3, it);
    it = new QTableWidgetItem(longitude);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 4, it);
    }

    it = new QTableWidgetItem(nmea);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 5, it);

    it = new QTableWidgetItem(ntripVersion);
    it->setFlags(it->flags() & ~Qt::ItemIsEditable);
    _mountPointsTable->setItem(iRow, 6, it);

    bncTableItem* bncIt = new bncTableItem();
    _mountPointsTable->setItem(iRow, 7, bncIt);

    iRow++;
  }
  _mountPointsTable->hideColumn(0);
  _mountPointsTable->sortItems(1);
  if (mountPoints->count() > 0 && !_actStop->isEnabled()) {
    _actGetData->setEnabled(true);
  }
  delete mountPoints;
}

// Save Options
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotSaveOptions() {

  QStringList mountPoints;
  for (int iRow = 0; iRow < _mountPointsTable->rowCount(); iRow++) {
    QUrl url( "//" + _mountPointsTable->item(iRow, 0)->text() + 
              "@"  + _mountPointsTable->item(iRow, 1)->text() );

    mountPoints.append(url.toString() + " " + 
                       _mountPointsTable->item(iRow, 2)->text()
               + " " + _mountPointsTable->item(iRow, 3)->text()
               + " " + _mountPointsTable->item(iRow, 4)->text()
               + " " + _mountPointsTable->item(iRow, 5)->text()
               + " " + _mountPointsTable->item(iRow, 6)->text());
  }

  bncSettings settings;

  settings.setValue("adviseFail",  _adviseFailSpinBox->value());
  settings.setValue("adviseReco",  _adviseRecoSpinBox->value());
  settings.setValue("adviseScript",_adviseScriptLineEdit->text());
  settings.setValue("autoStart",   _autoStartCheckBox->checkState());
  settings.setValue("binSampl",    _binSamplSpinBox->value());
  settings.setValue("corrIntr",    _corrIntrComboBox->currentText());
  settings.setValue("corrPath",    _corrPathLineEdit->text());
  settings.setValue("corrPort",    _corrPortLineEdit->text());
  settings.setValue("corrTime",    _corrTimeSpinBox->value());
  settings.setValue("ephIntr",     _ephIntrComboBox->currentText());
  settings.setValue("ephPath",     _ephPathLineEdit->text());
  settings.setValue("ephV3",       _ephV3CheckBox->checkState());
  settings.setValue("logFile",     _logFileLineEdit->text());
  settings.setValue("miscMount",   _miscMountLineEdit->text());
  settings.setValue("mountPoints", mountPoints);
  settings.setValue("obsRate",     _obsRateComboBox->currentText());
  settings.setValue("onTheFlyInterval", _onTheFlyComboBox->currentText());
  settings.setValue("outEphPort",  _outEphPortLineEdit->text());
  settings.setValue("outFile",     _outFileLineEdit->text());
  settings.setValue("outPort",     _outPortLineEdit->text());
  settings.setValue("outUPort",    _outUPortLineEdit->text());
  settings.setValue("perfIntr",    _perfIntrComboBox->currentText());
  settings.setValue("proxyHost",   _proxyHostLineEdit->text());
  settings.setValue("proxyPort",   _proxyPortLineEdit->text());
  settings.setValue("rnxAppend",   _rnxAppendCheckBox->checkState());
  settings.setValue("rnxIntr",     _rnxIntrComboBox->currentText());
  settings.setValue("rnxPath",     _rnxPathLineEdit->text());
  settings.setValue("rnxSampl",    _rnxSamplSpinBox->value());
  settings.setValue("rnxScript",   _rnxScrpLineEdit->text());
  settings.setValue("rnxSkel",     _rnxSkelLineEdit->text());
  settings.setValue("rnxV3",       _rnxV3CheckBox->checkState());
  settings.setValue("scanRTCM",    _scanRTCMCheckBox->checkState());
  settings.setValue("serialFileNMEA",_serialFileNMEALineEdit->text());
  settings.setValue("serialHeightNMEA",_serialHeightNMEALineEdit->text());
  settings.setValue("serialAutoNMEA",  _serialAutoNMEAComboBox->currentText());
  settings.setValue("serialBaudRate",  _serialBaudRateComboBox->currentText());
  settings.setValue("serialDataBits",  _serialDataBitsComboBox->currentText());
  settings.setValue("serialMountPoint",_serialMountPointLineEdit->text());
  settings.setValue("serialParity",    _serialParityComboBox->currentText());
  settings.setValue("serialPortName",  _serialPortNameLineEdit->text());
  settings.setValue("serialStopBits",  _serialStopBitsComboBox->currentText());
  settings.setValue("startTab",    aogroup->currentIndex());
  settings.setValue("waitTime",    _waitTimeSpinBox->value());

  if (_caster) {
    _caster->slotReadMountPoints();
  }
  settings.sync();
}

// All get slots terminated
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotGetThreadsFinished() {
  ((bncApp*)qApp)->slotMessage("All Get Threads Terminated", true);
  delete _caster; _caster = 0;
  _actGetData->setEnabled(true);
  _actStop->setEnabled(false);
}

// Retrieve Data
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotGetData() {
  slotSaveOptions();

  _actDeleteMountPoints->setEnabled(false);
  _actGetData->setEnabled(false);
  _actStop->setEnabled(true);

  _caster = new bncCaster(_outFileLineEdit->text(), 
                          _outPortLineEdit->text().toInt());

  ((bncApp*)qApp)->setPort(_outEphPortLineEdit->text().toInt());
  ((bncApp*)qApp)->setPortCorr(_corrPortLineEdit->text().toInt());

  connect(_caster, SIGNAL(getThreadsFinished()), 
          this, SLOT(slotGetThreadsFinished()));

  connect (_caster, SIGNAL(mountPointsRead(QList<bncGetThread*>)), 
           this, SLOT(slotMountPointsRead(QList<bncGetThread*>)));

  ((bncApp*)qApp)->slotMessage("============ Start BNC ============", true);

  _caster->slotReadMountPoints();
}

// Retrieve Data
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotStop() {
  int iRet = QMessageBox::question(this, "Stop", "Stop retrieving data?", 
                                   QMessageBox::Yes, QMessageBox::No,
                                   QMessageBox::NoButton);
  if (iRet == QMessageBox::Yes) {
    delete _caster; _caster = 0;
    _actGetData->setEnabled(true);
    _actStop->setEnabled(false);
  }
}

// Close Application gracefully
////////////////////////////////////////////////////////////////////////////
void bncWindow::closeEvent(QCloseEvent* event) {

  int iRet = QMessageBox::question(this, "Close", "Save Options?", 
                                   QMessageBox::Yes, QMessageBox::No,
                                   QMessageBox::Cancel);

  if      (iRet == QMessageBox::Cancel) {
    event->ignore();
    return;
  }
  else if (iRet == QMessageBox::Yes) {
    slotSaveOptions();
  }

  QMainWindow::closeEvent(event);
}

// User changed the selection of mountPoints
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotSelectionChanged() {
  if (_mountPointsTable->selectedItems().isEmpty()) {
    _actDeleteMountPoints->setEnabled(false);
  }
  else {
    _actDeleteMountPoints->setEnabled(true);
  }
}

// Display Program Messages 
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotWindowMessage(const QByteArray msg, bool showOnScreen) {

#ifdef DEBUG_RTCM2_2021  
  const int maxBufferSize = 1000;
#else
  const int maxBufferSize = 10000;
#endif

  if (! showOnScreen ) {
    return;
  }
 
  QString txt = _log->toPlainText() + "\n" + 
     QDateTime::currentDateTime().toUTC().toString("yy-MM-dd hh:mm:ss ") + msg;
  _log->clear();
  _log->append(txt.right(maxBufferSize));
}  

// About Message
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotAbout() {
 new bncAboutDlg(0);
}

//Flowchart
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotFlowchart() {
 new bncFlowchartDlg(0);
}

// Help Window
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotHelp() {
  QUrl url; 
  url.setPath(":bnchelp.html");
  new bncHlpDlg(0, url);
}

// Select Fonts
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotFontSel() {
  bool ok;
  QFont newFont = QFontDialog::getFont(&ok, this->font(), this); 
  if (ok) {
    bncSettings settings;
    settings.setValue("font", newFont.toString());
    QApplication::setFont(newFont);
    int ww = QFontMetrics(newFont).width('w');
    setMinimumSize(60*ww, 80*ww);
    resize(60*ww, 80*ww);
  }
}

// Whats This Help
void bncWindow::slotWhatsThis() {
  QWhatsThis::enterWhatsThisMode();
}

// 
////////////////////////////////////////////////////////////////////////////
void bncWindow::slotMountPointsRead(QList<bncGetThread*> threads) {
  populateMountPointsTable();
  bncSettings settings;
  _binSamplSpinBox->setValue(settings.value("binSampl").toInt());
  _waitTimeSpinBox->setValue(settings.value("waitTime").toInt());
  QListIterator<bncGetThread*> iTh(threads);
  while (iTh.hasNext()) {
    bncGetThread* thread = iTh.next();
    for (int iRow = 0; iRow < _mountPointsTable->rowCount(); iRow++) {
      QUrl url( "//" + _mountPointsTable->item(iRow, 0)->text() + 
                "@"  + _mountPointsTable->item(iRow, 1)->text() );
      if (url                                      == thread->mountPoint() &&
          _mountPointsTable->item(iRow, 3)->text() == thread->latitude()   &&
          _mountPointsTable->item(iRow, 4)->text() == thread->longitude() ) {
        ((bncTableItem*) _mountPointsTable->item(iRow, 7))->setGetThread(thread);
        break;
      }
    }
  }
}

// 
////////////////////////////////////////////////////////////////////////////
void bncWindow::CreateMenu() {
  // Create Menus
  // ------------
  _menuFile = menuBar()->addMenu(tr("&File"));
  _menuFile->addAction(_actFontSel);
  _menuFile->addSeparator();
  _menuFile->addAction(_actSaveOpt);
  _menuFile->addSeparator();
  _menuFile->addAction(_actQuit);

  _menuHlp = menuBar()->addMenu(tr("&Help"));
  _menuHlp->addAction(_actHelp);
  _menuHlp->addAction(_actFlowchart);
  _menuHlp->addAction(_actAbout);
}

// Toolbar
////////////////////////////////////////////////////////////////////////////
void bncWindow::AddToolbar() {
  // Tool (Command) Bar
  // ------------------
  QToolBar* toolBar = new QToolBar;
  addToolBar(Qt::BottomToolBarArea, toolBar); 
  toolBar->setMovable(false);
  toolBar->addAction(_actAddMountPoints);
  toolBar->addAction(_actDeleteMountPoints);
  toolBar->addAction(_actGetData);
  toolBar->addAction(_actStop);
  toolBar->addWidget(new QLabel("                                   "));
  toolBar->addAction(_actwhatsthis);
} 

// About
////////////////////////////////////////////////////////////////////////////
bncAboutDlg::bncAboutDlg(QWidget* parent) : 
   QDialog(parent) {

  QTextBrowser* tb = new QTextBrowser;
  QUrl url; url.setPath(":bncabout.html");
  tb->setSource(url);
  tb->setReadOnly(true);

  int ww = QFontMetrics(font()).width('w');
  QPushButton* _closeButton = new QPushButton("Close");
  _closeButton->setMaximumWidth(10*ww);
  connect(_closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QGridLayout* dlgLayout = new QGridLayout();
  QLabel* img = new QLabel();
  img->setPixmap(QPixmap(":ntrip-logo.png"));
  dlgLayout->addWidget(img, 0,0);
  dlgLayout->addWidget(new QLabel("BKG Ntrip Client (BNC) Version 1.7"), 0,1);
  dlgLayout->addWidget(tb,1,0,1,2);
  dlgLayout->addWidget(_closeButton,2,1,Qt::AlignRight);  

  setLayout(dlgLayout);
  resize(60*ww, 60*ww);
  setWindowTitle("About BNC");
  show();
}

// 
////////////////////////////////////////////////////////////////////////////
bncAboutDlg::~bncAboutDlg() {
}; 

// Flowchart 
////////////////////////////////////////////////////////////////////////////
bncFlowchartDlg::bncFlowchartDlg(QWidget* parent) :
   QDialog(parent) {

  int ww = QFontMetrics(font()).width('w');
  QPushButton* _closeButton = new QPushButton("Close");
  _closeButton->setMaximumWidth(10*ww);
  connect(_closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QGridLayout* dlgLayout = new QGridLayout();
  QLabel* img = new QLabel();
  img->setPixmap(QPixmap(":bncflowchart.png"));
  dlgLayout->addWidget(img, 0,0);
  dlgLayout->addWidget(_closeButton,1,0,Qt::AlignLeft);

  setLayout(dlgLayout);
  setWindowTitle("Flow Chart");
  show();
}

// 
////////////////////////////////////////////////////////////////////////////
bncFlowchartDlg::~bncFlowchartDlg() {
};

//  Bnc Text
////////////////////////////////////////////////////////////////////////////
void bncWindow::bncText(const QString &text){

  bool isEmpty = text.isEmpty();

  QPalette palette;
  QColor lightGray(230, 230, 230);
  QColor white(255, 255, 255);

  // Proxy
  //------
  if (aogroup->currentIndex() == 0) {
    if (!isEmpty) {
      _proxyPortLineEdit->setStyleSheet("background-color: white");
      _proxyPortLineEdit->setEnabled(true);
    } else {
      _proxyPortLineEdit->setStyleSheet("background-color: lightGray");
      _proxyPortLineEdit->setEnabled(false);
    }
  }

  // RINEX Observations
  // ------------------
  if (aogroup->currentIndex() == 2) {
    if (!isEmpty) {
      _rnxSamplSpinBox->setStyleSheet("background-color: white");
      _rnxSkelLineEdit->setStyleSheet("background-color: white");
      _rnxScrpLineEdit->setStyleSheet("background-color: white");
      palette.setColor(_rnxV3CheckBox->backgroundRole(), white);
      _rnxV3CheckBox->setPalette(palette);
      _rnxIntrComboBox->setStyleSheet("background-color: white");
      _rnxSamplSpinBox->setEnabled(true);
      _rnxSkelLineEdit->setEnabled(true);
      _rnxScrpLineEdit->setEnabled(true);
      _rnxV3CheckBox->setEnabled(true);
      _rnxIntrComboBox->setEnabled(true);
    } else {
      _rnxSamplSpinBox->setStyleSheet("background-color: lightGray");
      _rnxSkelLineEdit->setStyleSheet("background-color: lightGray");
      _rnxScrpLineEdit->setStyleSheet("background-color: lightGray");
      palette.setColor(_rnxV3CheckBox->backgroundRole(), lightGray);
      _rnxV3CheckBox->setPalette(palette);
      _rnxIntrComboBox->setStyleSheet("background-color: lightGray");
      _rnxSamplSpinBox->setEnabled(false);
      _rnxSkelLineEdit->setEnabled(false);
      _rnxScrpLineEdit->setEnabled(false);
      _rnxV3CheckBox->setEnabled(false);
      _rnxIntrComboBox->setEnabled(false);
    }
  }

  // RINEX Ephemeris
  // ---------------
  if (aogroup->currentIndex() == 3) {
    if (!_ephPathLineEdit->text().isEmpty() && !_outEphPortLineEdit->text().isEmpty()) { 
      _ephIntrComboBox->setStyleSheet("background-color: white");
      palette.setColor(_ephV3CheckBox->backgroundRole(), white);
      _ephV3CheckBox->setPalette(palette);
      _ephIntrComboBox->setEnabled(true);
      _ephV3CheckBox->setEnabled(true);
    }
    if (_ephPathLineEdit->text().isEmpty() && !_outEphPortLineEdit->text().isEmpty()) { 
      _ephIntrComboBox->setStyleSheet("background-color: lightGray");
      palette.setColor(_ephV3CheckBox->backgroundRole(), white);
      _ephV3CheckBox->setPalette(palette);
      _ephIntrComboBox->setEnabled(false);
      _ephV3CheckBox->setEnabled(true);
    }
    if (!_ephPathLineEdit->text().isEmpty() && _outEphPortLineEdit->text().isEmpty()) { 
      _ephIntrComboBox->setStyleSheet("background-color: white");
      palette.setColor(_ephV3CheckBox->backgroundRole(), white);
      _ephV3CheckBox->setPalette(palette);
      _ephIntrComboBox->setEnabled(true);
      _ephV3CheckBox->setEnabled(true);
    }
    if (_ephPathLineEdit->text().isEmpty() && _outEphPortLineEdit->text().isEmpty()) { 
      _ephIntrComboBox->setStyleSheet("background-color: lightGray");
      palette.setColor(_ephV3CheckBox->backgroundRole(), lightGray);
      _ephV3CheckBox->setPalette(palette);
      _ephIntrComboBox->setEnabled(false);
      _ephV3CheckBox->setEnabled(false);
    }
  }

  // Ephemeris Corrections
  // ---------------------
  if (aogroup->currentIndex() == 4) {
    if (!isEmpty) {
      if (!_corrPathLineEdit->text().isEmpty()) {
      _corrIntrComboBox->setStyleSheet("background-color: white");
      _corrPortLineEdit->setStyleSheet("background-color: white");
      _corrIntrComboBox->setEnabled(true);
      _corrPortLineEdit->setEnabled(true);
      }
      if (!_corrPortLineEdit->text().isEmpty()) {
      _corrTimeSpinBox->setStyleSheet("background-color: white");
      _corrTimeSpinBox->setEnabled(true);
      }
    } else {
      if (_corrPathLineEdit->text().isEmpty()) {
      _corrIntrComboBox->setStyleSheet("background-color: lightGray");
      _corrIntrComboBox->setEnabled(false); 
      }
      if (_corrPortLineEdit->text().isEmpty()) {
      _corrTimeSpinBox->setStyleSheet("background-color: lightGray");
      _corrTimeSpinBox->setEnabled(false);
      }
    }
  }

  // Feed Engine
  // -----------
  if (aogroup->currentIndex() == 5) {
    if ( !_outPortLineEdit->text().isEmpty() || !_outFileLineEdit->text().isEmpty()) {
      _waitTimeSpinBox->setStyleSheet("background-color: white");
      _binSamplSpinBox->setStyleSheet("background-color: white");
      _waitTimeSpinBox->setEnabled(true);
      _binSamplSpinBox->setEnabled(true);
    } else {
      _waitTimeSpinBox->setStyleSheet("background-color: lightGray");
      _binSamplSpinBox->setStyleSheet("background-color: lightGray");
      _waitTimeSpinBox->setEnabled(false);
      _binSamplSpinBox->setEnabled(false); 
    }
  }

  // Serial Link
  // -----------
  if (aogroup->currentIndex() == 6) {
    if (!isEmpty) {
      _serialPortNameLineEdit->setStyleSheet("background-color: white");
      _serialBaudRateComboBox->setStyleSheet("background-color: white");
      _serialParityComboBox->setStyleSheet("background-color: white");
      _serialDataBitsComboBox->setStyleSheet("background-color: white");
      _serialStopBitsComboBox->setStyleSheet("background-color: white");
      _serialAutoNMEAComboBox->setStyleSheet("background-color: white");
      _serialPortNameLineEdit->setEnabled(true);
      _serialBaudRateComboBox->setEnabled(true);
      _serialParityComboBox->setEnabled(true);
      _serialDataBitsComboBox->setEnabled(true);
      _serialStopBitsComboBox->setEnabled(true);
      _serialAutoNMEAComboBox->setEnabled(true);
      if (_serialAutoNMEAComboBox->currentText() != "Auto" ) {
        _serialHeightNMEALineEdit->setStyleSheet("background-color: white");
        _serialHeightNMEALineEdit->setEnabled(true);
        _serialFileNMEALineEdit->setStyleSheet("background-color: lightGray");
        _serialFileNMEALineEdit->setEnabled(false);
      } else {
        _serialHeightNMEALineEdit->setStyleSheet("background-color: lightGray");
        _serialHeightNMEALineEdit->setEnabled(false);
        _serialFileNMEALineEdit->setStyleSheet("background-color: white");
        _serialFileNMEALineEdit->setEnabled(true);
      }
    } else {
      _serialPortNameLineEdit->setStyleSheet("background-color: lightGray");
      _serialBaudRateComboBox->setStyleSheet("background-color: lightGray");
      _serialParityComboBox->setStyleSheet("background-color: lightGray");
      _serialDataBitsComboBox->setStyleSheet("background-color: lightGray");
      _serialStopBitsComboBox->setStyleSheet("background-color: lightGray");
      _serialAutoNMEAComboBox->setStyleSheet("background-color: lightGray");
      _serialFileNMEALineEdit->setStyleSheet("background-color: lightGray");
      _serialHeightNMEALineEdit->setStyleSheet("background-color: lightGray");
      _serialPortNameLineEdit->setEnabled(false);
      _serialBaudRateComboBox->setEnabled(false);
      _serialParityComboBox->setEnabled(false);
      _serialDataBitsComboBox->setEnabled(false);
      _serialStopBitsComboBox->setEnabled(false);
      _serialAutoNMEAComboBox->setEnabled(false);
      _serialHeightNMEALineEdit->setEnabled(false);
      _serialFileNMEALineEdit->setEnabled(false);
    }
  }

  // Outages
  // -------
  if (aogroup->currentIndex() == 7) {
    if (!isEmpty) {
      _adviseScriptLineEdit->setStyleSheet("background-color: white");
      _adviseFailSpinBox->setStyleSheet("background-color: white");
      _adviseRecoSpinBox->setStyleSheet("background-color: white");
      _adviseFailSpinBox->setEnabled(true);
      _adviseRecoSpinBox->setEnabled(true);
      _adviseScriptLineEdit->setEnabled(true);
    } else {
      _adviseScriptLineEdit->setStyleSheet("background-color: lightGray");
      _adviseFailSpinBox->setStyleSheet("background-color: lightGray");
      _adviseRecoSpinBox->setStyleSheet("background-color: lightGray");
      _adviseFailSpinBox->setEnabled(false);
      _adviseRecoSpinBox->setEnabled(false);
      _adviseScriptLineEdit->setEnabled(false);
    }
  }

  // Miscellaneous
  // -------------
  if (aogroup->currentIndex() == 8) {
    if (!isEmpty) {
      _perfIntrComboBox->setStyleSheet("background-color: white");
      palette.setColor(_scanRTCMCheckBox->backgroundRole(), white);
      _scanRTCMCheckBox->setPalette(palette);
      _perfIntrComboBox->setEnabled(true);
      _scanRTCMCheckBox->setEnabled(true);
    } else {
      _perfIntrComboBox->setStyleSheet("background-color: lightGray");
      palette.setColor(_scanRTCMCheckBox->backgroundRole(), lightGray);
      _scanRTCMCheckBox->setPalette(palette);
      _perfIntrComboBox->setEnabled(false);
      _scanRTCMCheckBox->setEnabled(false);
    }
  }
}

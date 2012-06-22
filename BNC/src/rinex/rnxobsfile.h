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

#ifndef RNXOBSFILE_H
#define RNXOBSFILE_H

#include <QtCore>

#include <fstream>
#include <vector>
#include <map>

#include "newmat.h"
#include "bncconst.h"
#include "bnctime.h"

#define MAXPRN_GPS 32

class t_rnxObsFile {
 public:

  static bool earlierStartTime(const t_rnxObsFile* file1, const t_rnxObsFile* file2) {
    return file1->startTime() < file2->startTime();
  }

  class t_rnxSat {
   public:
    char                satSys;
    int                 satNum;
    std::vector<double> obs;
    std::vector<int>    lli;
    std::vector<int>    snr;
  };

  class t_rnxEpo {
   public:
    void clear() {
      rnxSat.clear();
    }
    bncTime               tt;
    std::vector<t_rnxSat> rnxSat;
  };

  enum e_inpOut {input, output};

  t_rnxObsFile(const QString& fileName, e_inpOut inpOut);
  ~t_rnxObsFile();
  
  float               version() const {return _header._version;}
  double              interval() const {return _header._interval;}
  int                 nTypes(char sys) const {return _header.nTypes(sys);}
  const QString&      fileName() const {return _fileName;}
  const QString&      obsType(char sys, int index) const {return _header.obsType(sys, index);}

  const QString&      antennaName() const {return _header._antennaName;}
  const QString&      markerName() const {return _header._markerName;}
  const QString&      receiverType() const {return _header._receiverType;}

  void setInterval(double interval) {_header._interval = interval;}
  void setAntennaName(const QString& antennaName) {_header._antennaName = antennaName;}
  void setMarkerName(const QString& markerName) {_header._markerName = markerName;}
  void setReceiverType(const QString& receiverType) {_header._receiverType = receiverType;}

  const ColumnVector& xyz() const {return _header._xyz;}
  const ColumnVector& antNEU() const {return _header._antNEU;}
  const ColumnVector& antXYZ() const {return _header._antXYZ;}
  const ColumnVector& antBSG() const {return _header._antBSG;}

  const bncTime&      startTime() const {return _header._startTime;}
  void  setStartTime(const bncTime& startTime) {_header._startTime = startTime;}

  t_rnxEpo* nextEpoch(); 
  int wlFactorL1(unsigned iPrn) {
    return iPrn <= MAXPRN_GPS ? _header._wlFactorsL1[iPrn] : 1;
  }
  int wlFactorL2(unsigned iPrn) {
    return iPrn <= MAXPRN_GPS ? _header._wlFactorsL2[iPrn] : 1;
  }

 protected:
  t_rnxObsFile() {};
  void openRead(const QString& fileName);
  void openWrite(const QString& fileName);
  void close();

 private: 
  class t_rnxObsHeader {
   public:
    t_rnxObsHeader();
    ~t_rnxObsHeader();

    t_irc           read(QTextStream* stream, int maxLines = 0);
    int             nTypes(char sys) const;
    const QString&  obsType(char sys, int index) const;
 
    static const QString          _emptyStr;
    float                         _version;
    double                        _interval;
    QString                       _antennaNumber;
    QString                       _antennaName;
    QString                       _markerName;
    QString                       _markerNumber;
    QString                       _observer;
    QString                       _agency;
    QString                       _receiverNumber;
    QString                       _receiverType;
    QString                       _receiverVersion;
    ColumnVector                  _antNEU;
    ColumnVector                  _antXYZ;
    ColumnVector                  _antBSG;
    ColumnVector                  _xyz;
    QVector<QString>              _obsTypesV2;
    QMap<char, QVector<QString> > _obsTypesV3;
    int                           _wlFactorsL1[MAXPRN_GPS+1];
    int                           _wlFactorsL2[MAXPRN_GPS+1];
    bncTime                       _startTime;
  };

 public:
  const t_rnxObsHeader& header() const {return _header;}
  void setHeader(const t_rnxObsHeader& header, double version);
  void checkNewHeader(const t_rnxObsHeader& header);
  void writeHeader(const QMap<QString, QString>* txtMap = 0);
  QStringList obsTypesStrings();
  void writeEpoch(const t_rnxEpo* epo);

 private:
  enum e_trafo {trafoNone, trafo2to3, trafo3to2};

  void writeEpochV2(const t_rnxEpo* epo);
  void writeEpochV3(const t_rnxEpo* epo);
  t_rnxEpo* nextEpochV2();
  t_rnxEpo* nextEpochV3();
  void handleEpochFlag(int flag, const QString& line);

  QString type2to3(char sys, const QString& typeV2);
  QString type3to2(const QString& typeV3);

  QMap<char, QMap<int, int> > _indexMap2to3;
  QMap<char, QMap<int, int> > _indexMap3to2;

  e_inpOut       _inpOut;
  QFile*         _file;
  QString        _fileName;
  QTextStream*   _stream;
  t_rnxObsHeader _header;
  t_rnxEpo       _currEpo;
  bool           _flgPowerFail;
  e_trafo        _trafo;
};

#endif
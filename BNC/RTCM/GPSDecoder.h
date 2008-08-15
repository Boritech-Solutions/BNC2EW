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

#ifndef GPSDECODER_H
#define GPSDECODER_H

#include <iostream>
#include <QPointer>
#include <QList>

#include "bncconst.h"

class t_obsInternal {
 public:
  int    flags;
  char   StatID[20+1];// Station ID
  char   satSys;      // Satellite System ('G' or 'R')
  int    satNum;      // Satellite Number (PRN for GPS NAVSTAR)
  int    slot;        // Slot Number (for Glonass)
  int    GPSWeek;     // Week of GPS-Time
  double GPSWeeks;    // Second of Week (GPS-Time)
  double C1;          // CA-code pseudorange (meters)
  double C2;          // CA-code pseudorange (meters)
  double P1;          // P1-code pseudorange (meters)
  double P2;          // P2-code pseudorange (meters)
  double L1;          // L1 carrier phase (cycles)
  double L2;          // L2 carrier phase (cycles)
  double S1;          // L1 signal-to noise ratio
  double S2;          // L2 signal-to noise ratio
  int    SNR1;        // L1 signal-to noise ratio (mapped to integer)
  int    SNR2;        // L2 signal-to noise ratio (mapped to integer)
};

class t_obs : public QObject{
 public:
  enum t_obs_status {initial, posted, received};

  t_obs() {
    _status = initial;

    _o.flags     = 0;
    _o.StatID[0] = '\0';
    _o.satSys    = 'G';
    _o.satNum    = 0;
    _o.slot      = 0;
    _o.GPSWeek   = 0;
    _o.GPSWeeks  = 0.0;
    _o.C1        = 0.0;
    _o.C2        = 0.0;
    _o.P1        = 0.0;
    _o.P2        = 0.0;
    _o.L1        = 0.0;
    _o.L2        = 0.0;
    _o.S1        = 0.0;
    _o.S2        = 0.0;
    _o.SNR1      = 0;
    _o.SNR2      = 0;
  }

  ~t_obs() {}

  t_obsInternal _o;
  t_obs_status  _status;
};

typedef QPointer<t_obs> p_obs;

class GPSDecoder {
 public:
  virtual t_irc Decode(char* buffer, int bufLen) = 0;

  virtual ~GPSDecoder() {
    QListIterator<p_obs> it(_obsList);
    while (it.hasNext()) {
      p_obs obs = it.next();
      if (!obs.isNull() && obs->_status == t_obs::initial) {
        delete obs;
      }
    }
  }

  QList<p_obs> _obsList;
  QList<int> _typeList; // RTCM message types
  QList<int> _epochList; // RTCM message types
};

#endif

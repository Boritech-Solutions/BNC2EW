
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

#include "thriftclient.h"
#include "map_stations.h"

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

using namespace com::gpssolutions::rtnet;
using namespace std;
using namespace boost;

// Constructor
//////////////////////////////////////////////////////////////////////////////
t_thriftClient::t_thriftClient(GnssCenter::t_map_stations* parent) {
  _stop   = false;
  _parent = parent;
}

// Destructor
//////////////////////////////////////////////////////////////////////////////
t_thriftClient::~t_thriftClient() {
}

// Run (virtual)
//////////////////////////////////////////////////////////////////////////////
void t_thriftClient::run() {

  string host = "rtnet.rtcm-ntrip.org";
  int    port = 7777;

  shared_ptr<TSocket>     socket(new TSocket(host, port));
  shared_ptr<TTransport>  transport(new TBufferedTransport(socket)); 
  shared_ptr<TProtocol>   protocol(new TBinaryProtocol(transport));
  shared_ptr<RtnetDataIf> dataHandler(new t_thriftHandler(_parent));
  shared_ptr<TProcessor>  processor(new RtnetDataProcessor(dataHandler));

  try {
    transport->open();
    while (true) {
      {
        QMutexLocker locker(&_mutex);
        if (_stop) {
          break;
        }
      }
      if (processor->process(protocol,protocol,0) == 0) {
        break;
      }
    }
    transport->close();
  } 
  catch (TException& e) {
    cerr << "Caught an exception generated by Thrift: " << e.what() << endl;
  } 
  catch (...) {
    cerr << "Unknown exception" << endl;
  }
}

// Constructor
//////////////////////////////////////////////////////////////////////////////
t_thriftHandler::t_thriftHandler(GnssCenter::t_map_stations* parent) {
  _parent = parent;
}

// Destructor
//////////////////////////////////////////////////////////////////////////////
t_thriftHandler::~t_thriftHandler() {
}

// Handle Satellite Positions
//////////////////////////////////////////////////////////////////////////////
void t_thriftHandler::
handleSatelliteXYZ(const vector<SatelliteXYZ>& svXYZList) {
  cout.setf(ios::fixed);
  for (unsigned ii = 0; ii < svXYZList.size(); ii++) {
//    const SatelliteXYZ& sat = svXYZList[ii];
//    cout << unsigned(sat.ID) << ' '
//         << setprecision(3) << sat.xyz.x << ' '
//         << setprecision(3) << sat.xyz.y << ' '
//         << setprecision(3) << sat.xyz.z << endl;
  }
//  cout << endl;
}

// Handle Station Info
//////////////////////////////////////////////////////////////////////////////
void t_thriftHandler::
handleStationInfo(const vector<StationInfo>& stationList) {
  for (unsigned ii = 0; ii < stationList.size(); ii++) {
    const StationInfo& staInfo = stationList[ii];
    _stationCrd[staInfo.ID]._x = staInfo.xyz.x;
    _stationCrd[staInfo.ID]._y = staInfo.xyz.y;
    _stationCrd[staInfo.ID]._z = staInfo.xyz.z;
  }
}

// Handle Epoch Results
//////////////////////////////////////////////////////////////////////////////
void t_thriftHandler::
handleEpochResults(const RtnetEpoch& epoch) {
  vector<t_thriftResult*>* results = new vector<t_thriftResult*>;
  for (unsigned ii = 0; ii < epoch.stationResultList.size(); ii++) {
    const StationResults& staRes = epoch.stationResultList[ii];
    t_thriftResult* res = new t_thriftResult;
    res->_name = staRes.stationName;
    res->_nGPS = staRes.nsv_gps_used;
    res->_nGLO = staRes.nsv_glonass_used;
    if (_stationCrd.find(staRes.stationName) != _stationCrd.end()) {
      res->_x = _stationCrd[staRes.stationName]._x;
      res->_y = _stationCrd[staRes.stationName]._y;
      res->_z = _stationCrd[staRes.stationName]._z;
    }
    results->push_back(res);
  }
  _parent->putThriftResults(results);
}
#ifndef rooutil_Trigger_hh_
#define rooutil_Trigger_hh_

#include <functional>
#include <map>
#include "EventNtuple/inc/TrigInfo.hh"

namespace rooutil {
  struct Trigger {
    Trigger(mu2e::TrigInfo* trig = nullptr)
      : _trig(trig) {

    }

    void Update(bool debug = false) {
    }

    void SetTrigInfo(const mu2e::TrigInfo* trig) { _trig = trig; }

    void AssignIndex(const int index, const std::string& name) {
      _indexToName[index] = name;
      _nameToIndex[name]  = index;
    }

    int GetIndex(const std::string& name) const {
      if(_nameToIndex.count(name) == 0) return -1;
      return _nameToIndex.at(name);
    }

    bool Fired(const int index) const {
      if(!_trig) return false;
      if(index >= int(mu2e::TrigInfo::ntrig_)) return false;
      if(_indexToName.count(index) == 0) return false; // not in the trigger menu
      return _trig->_triggerArray[index];
    }

    bool Fired(const std::string& name) const {
      if(_nameToIndex.count(name) == 0) return false;
      return Fired(_nameToIndex.at(name));
    }

    // Check if any trigger with this pattern string was fired
    bool FiredByTag(const std::string pattern) const {
      bool fired = false;
      for(const auto& itr : NameToIndexMap()) {
        const std::string name = itr.first;
        if(name.find(pattern) != std::string::npos) fired |= Fired(itr.second);
        if(fired) break;
      }
      return fired;
    }

    // Data accessors
    const mu2e::TrigInfo*             TrigInfo      () const { return _trig; }
    const std::map<std::string, int>& NameToIndexMap() const { return _nameToIndex; }
    const std::map<int, std::string>& IndexToNameMap() const { return _indexToName; }

    // Pointer to the data
    const mu2e::TrigInfo* _trig = nullptr;
    std::map<int, std::string> _indexToName;
    std::map<std::string, int> _nameToIndex;
  };

  typedef std::function<bool(Trigger&)> TriggerCut;
} // namespace rooutil
#endif

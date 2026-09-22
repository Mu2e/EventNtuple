#ifndef UserBranch_hh_
#define UserBranch_hh_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "TChain.h"
#include "TTree.h"

namespace rooutil {
  enum class UserBranchScope {
    Event,
    Track
  };

  class UserBranchBase {
    public:
      UserBranchBase(std::string branch_name, UserBranchScope branch_scope)
        : branch_name_(std::move(branch_name)), branch_scope_(branch_scope) {}
      virtual ~UserBranchBase() = default;

      const std::string& name() const { return branch_name_; }
      UserBranchScope scope() const { return branch_scope_; }
      bool is_bound() const { return is_bound_; }

      virtual bool Bind(TChain* ntuple) = 0;
      virtual void BranchOutput(TTree* tree) = 0;
      virtual void* EventPtr() { return nullptr; }
      virtual void* TrackElementPtr(std::size_t) { return nullptr; }
      virtual void EraseTrack(std::size_t) {}

    protected:
      std::string branch_name_;
      UserBranchScope branch_scope_;
      bool is_bound_ = false;
  };

  template <typename T>
  class EventUserBranch : public UserBranchBase {
    public:
      explicit EventUserBranch(const std::string& branch_name)
        : UserBranchBase(branch_name, UserBranchScope::Event) {}

      bool Bind(TChain* ntuple) override {
        if (ntuple == nullptr || ntuple->GetBranch(branch_name_.c_str()) == nullptr || ntuple->GetBranchStatus(branch_name_.c_str()) == 0) {
          is_bound_ = false;
          return false;
        }
        ntuple->SetBranchAddress(branch_name_.c_str(), &value_);
        is_bound_ = true;
        return true;
      }

      void BranchOutput(TTree* tree) override {
        if (tree != nullptr && value_ != nullptr) {
          tree->Branch(branch_name_.c_str(), value_);
        }
      }

      void* EventPtr() override { return value_; }
      T* value() { return value_; }

    private:
      T* value_ = nullptr;
  };

  template <typename T>
  class TrackUserBranch : public UserBranchBase {
    public:
      explicit TrackUserBranch(const std::string& branch_name)
        : UserBranchBase(branch_name, UserBranchScope::Track) {}

      bool Bind(TChain* ntuple) override {
        if (ntuple == nullptr || ntuple->GetBranch(branch_name_.c_str()) == nullptr || ntuple->GetBranchStatus(branch_name_.c_str()) == 0) {
          is_bound_ = false;
          return false;
        }
        ntuple->SetBranchAddress(branch_name_.c_str(), &values_);
        is_bound_ = true;
        return true;
      }

      void BranchOutput(TTree* tree) override {
        if (tree != nullptr && values_ != nullptr) {
          tree->Branch(branch_name_.c_str(), values_);
        }
      }

      void* TrackElementPtr(std::size_t index) override {
        if (values_ == nullptr || index >= values_->size()) {
          return nullptr;
        }
        return &(values_->at(index));
      }

      void EraseTrack(std::size_t index) override {
        if (values_ != nullptr && index < values_->size()) {
          values_->erase(values_->begin() + index);
        }
      }

      std::vector<T>* values() { return values_; }

    private:
      std::vector<T>* values_ = nullptr;
  };

  template <typename T>
  std::shared_ptr<EventUserBranch<T>> MakeEventUserBranch(const std::string& branch_name) {
    return std::make_shared<EventUserBranch<T>>(branch_name);
  }

  template <typename T>
  std::shared_ptr<TrackUserBranch<T>> MakeTrackUserBranch(const std::string& branch_name) {
    return std::make_shared<TrackUserBranch<T>>(branch_name);
  }
} // namespace rooutil

#endif

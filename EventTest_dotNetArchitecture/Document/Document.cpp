// これは メイン DLL ファイルです。

#include "stdafx.h"
#include "Document.h"

namespace trial {
  public ref class Document {
  private:
    int value_;
  public:
    event System::EventHandler^ UpdateAllViews;
	
    Document() : value_(0) {}
    
	void Add(int n) {
      value_ += n;
      UpdateAllViews(this, nullptr);
    }
    
	int Value() { return value_; }
  };
} 
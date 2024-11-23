#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include <set>
#include "../maincontext/enum_def.hpp"
#include "../symbolutil/SymbolTable.h"
#include <assert.h>

using namespace std;

class NodeBlockData;

class liveVarsNode;

class ASTNode
{
protected:
    ASTNode *parent;
    // string typeofNode;
    NODETYPE typeofNode;
    SymbolTable *var_symbTab;
    SymbolTable *prop_symbTab;
    NodeBlockData *blockData;
    liveVarsNode *liveVarData;
    set<string> uses, defs, decls;

public:
    ASTNode()
    {
        parent = NULL;
    }
    void setParent(ASTNode *node)
    {
        parent = node;
    }
    void setTypeofNode(NODETYPE type)
    {
        typeofNode = type;
    }
    NODETYPE getTypeofNode()
    {
        return typeofNode;
    }
    ASTNode *getParent()
    {
        return parent;
    }

    void createSymbTab()
    {
        var_symbTab = new SymbolTable(this, 0);
        prop_symbTab = new SymbolTable(this, 1);
    }

    SymbolTable *getVarSymbT()
    {
        return var_symbTab;
    }
    SymbolTable *getPropSymbT()
    {
        return prop_symbTab;
    }

    void setBlockData(NodeBlockData *blockData)
    {
        this->blockData = blockData;
    }

    NodeBlockData *getBlockData()
    {
        return blockData;
    }

    void setLiveVarsNode(liveVarsNode *node)
    {
        this->liveVarData = node;
    }

    liveVarsNode *getLiveVarsNode()
    {
        return liveVarData;
    }

    void setUses(set<string> uses)
    {
        this->uses.clear();
        this->uses.insert(uses.begin(), uses.end());
    }

    void setDefs(set<string> defs)
    {
        this->defs.clear();
        this->defs.insert(defs.begin(), defs.end());
    }

    void setDecls(set<string> decls)
    {
        this->decls.clear();
        this->decls.insert(decls.begin(), decls.end());
    }

    set<string> getUses()
    {
        return uses;
    }

    set<string> getDefs()
    {
        return defs;
    }

    set<string> getDecls()
    {
        return decls;
    }
};

#endif
=======
#ifndef ASTNODE_H
#define ASTNODE_H

#include <string>
#include "../maincontext/enum_def.hpp"
#include "../symbolutil/SymbolTable.h"
#include <assert.h>

using namespace std;

class NodeBlockData;

class ASTNode
{
protected:
  ASTNode *parent;
  // string typeofNode;
  NODETYPE typeofNode;
  SymbolTable *var_symbTab;
  SymbolTable *prop_symbTab;
  NodeBlockData *blockData;

public:
  ASTNode()
  {
    parent = NULL;
  }
  void setParent(ASTNode *node)
  {
    parent = node;
  }
  void setTypeofNode(NODETYPE type)
  {
    typeofNode = type;
  }
  NODETYPE getTypeofNode()
  {
    return typeofNode;
  }
  ASTNode *getParent()
  {
    return parent;
  }

  void createSymbTab()
  {
    var_symbTab = new SymbolTable(this, 0);
    prop_symbTab = new SymbolTable(this, 1);
  }

  SymbolTable *getVarSymbT()
  {
    return var_symbTab;
  }
  SymbolTable *getPropSymbT()
  {
    return prop_symbTab;
  }

  void setBlockData(NodeBlockData *blockData)
  {
    this->blockData = blockData;
  }

  NodeBlockData *getBlockData()
  {
    return blockData;
  }
};

#endif

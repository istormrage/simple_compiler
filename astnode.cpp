#include "astnode.h"

#include <cstdlib>
#include <map>
using namespace std;

#define PREORDER


#define PRINT_TREE_STRING

extern map<string, ASTNode*> func_map;

ASTNode *get_true_node()
{
  static ASTNode true_node(true_token);
  return &true_node;
}

std::map<std::string, ASTNode *> env;


void print_ast()
{
  ASTNode *root = get_root();
  cout << "===========================" << endl;
#ifdef PRINT_TREE_STRING
  cout << "\\tree";
  root->print_tree();
  cout << endl;
#else
  cout << "ast node type: " << root->type_str() << endl;
  root->print();
  cout << endl;
#endif
  cout << "---------------------------" << endl;
}

std::string ObjType::str()
{
  std::string s;
  if (pointer_)
    s += " |ptr<" + std::to_string(pointer_number_) + ">";
  if(char_)
    s += " |char";
  if(int_)
    s += " |int";
  if(func_)
    s += " |func";
  if(array_)
    s += " |array";
  if(global_)
    s += " |global";
  return s;
}

u32 ASTNode::no_=0;

string tree_string(const string &str)
{
    string print_str{str};
    string s1("%");
    string s2("(");
    string s3(")");
    char s4='\n';
    //cout << endl << "XXX:" << print_str << "YYY" << endl;
    if (print_str.find(s1) != string::npos)
    {
      print_str.replace(print_str.find(s1), s1.length(), R"(\%)");
      //cout << endl << "BBB:" << print_str << endl;
    }

    if (print_str.find(s2) != string::npos)
    {
      //cout << endl << "CCC:" << print_str << endl;
      print_str.replace(print_str.find(s2), s2.length(), R"(\()");
    }
    if (print_str.find(s3) != string::npos)
      print_str.replace(print_str.find(s3), s3.length(), R"(\))");
    if (print_str.find(s4) != string::npos)
    {
      print_str.replace(print_str.find(s4), 1, R"(\\n)");
      //cout << endl << "AAA:" << print_str << endl;
    }
  return print_str;
}

void ASTNode::print_tree()
{
  if (children_.size() == 0) // leaf node
  {
    string print_str = tree_string(token_.str_);
    cout << "(" << print_str;
    if (ast_type() == NAME)
      cout << obj_type_.str();

    cout << ")";
  }
  else
  {
  #if 0
    string str("one three two four");
    string str2("three");
    str.replace(str.find(str2),str2.length(),"five");
  #endif

    string print_str = tree_string(token_.str_);
    //cout << "( " << token_.str_;
    cout << "( " << print_str;
    if (ast_type() == NAME)
    {
      cout << obj_type_.str();
    }
    //for (std::vector<ASTNode*>::size_type i=0 ; i < children_.size() ; ++i)
    for (auto i : children_)
    {
      i->print_tree();
    }
    cout << ")";
  }
}

void ASTNode::print()
{
  if (children_.size() == 0) // leaf node
  {
    cout << token_.str_ << " ";
  }
  else
  {
    cout << "( ";
#ifdef PREORDER
    cout << token_.str_ << " "; // preorder
#endif
    for (std::vector<ASTNode*>::size_type i=0 ; i < children_.size() ; ++i)
    {
      children_[i]->print();
#ifdef INORDER
      if (i == 0)
        cout << token_.str_ << " "; // inorder
#endif
    }
#ifdef POSTORDER
    cout << token_.str_ << " "; // postorder 
#endif
    cout << ")";
  #if 0
    switch (type())
    {
      case ADD:
      {
        cout << "(";
        children_[0]->print();
        cout << " + ";
        children_[1]->print();
        cout << ")" << endl;
      }
      case MUL:
      {
        cout << "(";
        children_[0]->print();
        cout << " * ";
        children_[1]->print();
        cout << ")" << endl;
      }
    }
  #endif
  }
}

ASTNode* ASTNode::eval(Environment *env)
{
#if 1
  if (ast_type() == GLOBAL_VAR)
  {
    for (auto &i : children())
      env->add(i->str(), get_true_node());
    
    return this;
  }
  if (ast_type() == RETURN)
  {
    ASTNode *r;
    for (auto &i : children())
    {
      r = i->eval(env);
    }
    return r;
  }
  if (ast_type() == VAR) // var declare
  {
    //cout << "Xx env str:" << env->name() << endl;

    for (auto &i : children())
    {
      //cout << "add: " << i->str() << " to env: " << env->name() << endl;
      env->add(i->str(), get_true_node());
    }
    return this;
  }
#endif
  if (ast_type() == FUNC_CALL) 
  {
    if (children().size() > 0) // has arguments
    {
      // new env
      //Environment *env = new Environment();
    }

    
    ASTNode *f_node = func_map[str()];
    ASTNode *f_para = 0;
    ASTNode *f_body = 0;

    if (f_node)
    {

    if (f_node->children().size() == 1)
    {

      f_body = f_node->children()[0];


      //return f_body->eval();
      //cout << "xxx " << endl;
      //exit(0);
    }
    else if (f_node->children().size() == 2)
         {
           f_para = f_node->children()[0];
           f_body = f_node->children()[1];
         }
         else
         {
           // some error
           cout << "eval func error!!" << endl;
           exit(5);
         }
    }

    if (str()=="printf")
    {
      string cmd{R"(printf ")"};
      cmd += children()[0]->str();
      cmd += "\" ";

      auto arg_num =  children().size();

      for (decltype(arg_num) i=1 ; i < arg_num; ++i)
      {
        ASTNode *eval_node = children()[i]->eval(env);

        cmd += "\"";
        cmd += eval_node->str();
        cmd += "\" ";
      }
      cout << "cmd: " << cmd << endl;
      system(cmd.c_str());

      #if 0
      cout << "print arg: " << endl;
      for (auto &i : children())
      {
        cout << i->str() << endl;
      }
      switch (children().size())
      {
        case 0:
        {
          cout << "printf has no arguments" << endl;
          break;
        }
        case 1:
        {
          printf(children()[0]->str().c_str());
          break;
        }
        case 2:
        {
          //printf(children()[0]->str().c_str(), stoi(children()[1]->eval(env)->str()));
          if (children()[1]->eval(env)->ast_type() == STRING)
          {
            printf(children()[0]->str().c_str(), children()[1]->eval(env)->str().c_str());
          }
          else if (children()[1]->eval(env)->ast_type() == NUMBER)
               {
                 printf(children()[0]->str().c_str(), stoi(children()[1]->eval(env)->str()));
               }
               else
               {
                 cout << "only handle STRING/NUMBER" << endl;
               }
          break;
        }
        case 3:
        {
          ASTNode *arg1 = children()[1]->eval(env);
          ASTNode *arg2 = children()[2]->eval(env);

                  (arg1->ast_type() == STRING) ? arg1->str().c_str() : stoi(arg1->str().c_str());

#if 0
          printf(children()[0]->str().c_str(), 
                  (arg1->ast_type() == STRING) ? arg1->str().c_str() : stoi(arg1->str()), 
                  (arg2->ast_type() == STRING) ? arg2->str().c_str() : stoi(arg2->str())
                )
    #endif             
          break;
        }
        case 4:
        {
          printf(children()[0]->str().c_str(), stoi(children()[1]->eval(env)->str()), stoi(children()[2]->eval(env)->str()), stoi(children()[3]->eval(env)->str()));
          break;
        }
        case 5:
        {
          printf(children()[0]->str().c_str(), stoi(children()[1]->eval(env)->str()), stoi(children()[2]->eval(env)->str()), stoi(children()[3]->eval(env)->str()), stoi(children()[4]->eval(env)->str()));
          break;
        }
        case 6:
        {
          printf(children()[0]->str().c_str(), stoi(children()[1]->eval(env)->str()), stoi(children()[2]->eval(env)->str()), stoi(children()[3]->eval(env)->str()), stoi(children()[4]->eval(env)->str()), stoi(children()[5]->eval(env)->str()));
          break;
        }

      }
      #endif
      return get_true_node();
    }

      Environment *func_env = new Environment(env, str() + "_env");
      #if 0
      cout << "func_env->name(): ";
      cout << func_env->name() << endl;
      cout << "f_para" << endl;
      #endif
      if (f_para)
      {
        //if (children().size() > 0) // has arguments
        //for (auto &i : f_para->children())

        auto var_size = f_para->children().size();
        auto val_size = children().size();

        if (var_size == val_size)
        {
          for (auto i=0 ; i < f_para->children().size() ; ++i)
          {
            ASTNode *arg_node = children()[i];
            #if 0
            ASTNode *n = func_env->lookup(f_para->children()[i]->str());
            if (n)
            {
              func_env->edit(f_para->children()[i]->str(), arg_node->eval(func_env));
            }
            else
            #endif
            func_env->add(f_para->children()[i]->str(), arg_node->eval(func_env));
          }
        }
        else
        {
          cout << "ASTNode: " << str()<< ", var_size: " << var_size << ", val_size: " << val_size << " are not the same." << endl;
          exit(FUNC_PARA_ARGU_NOT_MATCH);
        }

        //f_para->print();
      }

      ASTNode *ret=0;
#if 0
      cout << "2222 " << endl;
      f_body->print();
      cout << "3333 " << endl;
#endif
      for (auto &i : f_body->children())
        ret = i->eval(func_env);
      return ret;
  }

  if (children_.size() == 0) // leaf node
  {
    if (ast_type() == NAME)
    {
      //cout << "lookup: " << str() << ", in env name: " << env->name() << endl;
      ASTNode *n = env->lookup(str());
      if (n)
      {
        //cout << "find" << endl;
        return n;
      }
      else
      {
        //cout << "not found" << endl;
        return this;
      }

    #if 0
      //if (env.count(str()))
      {
        //ASTNode *n = env[str()];
        //cout << "eval name '" << str() << "' : " << n->str() << endl;
        //return env[str()];
      }
      else
      #endif
        //return this;
    }
    else
      return this;
  }
  else
  {
    //cout << "cur node: " << str() << endl;

    //for (const auto &i : children_)
    {

    if (token_.str_ == "+" || token_.str_ == "-" || token_.str_ == "*")
    {
      if (children_.size() == 2)
      {
        ASTNode *c1 = children_[0]->eval(env);
        #ifdef DEBUG_MSG
        cout << "xx cur: " << str() << endl;
        cout << "c1: " << c1->str() << endl;
        #endif
        ASTNode *c2 = children_[1]->eval(env);
        #ifdef DEBUG_MSG
        cout << "c2: " << c2->str() << endl;
        #endif

#if 0
        if (children_[0]->eval_result_)
        {
          ASTNode *e = children_[0]->eval_result_;
          delete children_[0];
          children_[0] = e;
          print_ast();
        }

        if (children_[1]->eval_result_)
        {
          ASTNode *e = children_[1]->eval_result_;
          delete children_[1];
          children_[1] = e;
          print_ast();
        }
#endif

#if 0
        if (c1_leaf == false)
        { // replace child node by eval node;
          delete children_[0];
          children_[0] = c1;
          print_ast();
        }
        else if(c2_leaf == false)
             { // replace child node by eval node;
               delete children_[1];
               children_[1] = c2;
               print_ast();
             }

        //|| children_[1]->is_leaf() == false)
          //print_ast();
#endif
        int n1 = stoi(c1->str());
        int n2 = stoi(c2->str());
        int ret = 0;
        if (token_.str_ == "+")
          ret = n1 + n2;
        if (token_.str_ == "-")
          ret = n1 - n2;
        if (token_.str_ == "*")
          ret = n1 * n2;

        //printf("ret: %d = n1: %d %s n2: %d\n", ret, n1, str().c_str(), n2);
        #ifdef DEBUG_MSG
        cout << "eval: " << n1 << " " << str() << " " << n2 << " = " << ret << endl;
        #endif

#if 0
        set_str(std::to_string(ret));
        free_children();
        return this;

#else
        Token t(std::to_string(ret), NUMBER);
        //t.str_ = std::to_string(ret);
        eval_result_ = new ASTNode(t);

        //*this = *tmp;
        return eval_result_;
#endif
      }
      else
      {
        cout << "children_.size() != 2, " << children_.size() << endl;
        return nullptr;
      }
    }
    else if (str() == "=")
         {
        #ifdef DEBUG_MSG
           cout << "op is =" << endl;
        #endif
           // add var/val to env
           ASTNode *c1 = children_[1]->eval(env);

           ASTNode *c0 = children_[0];

           if (c0->ast_type() != NAME)
           {
             cout << "= error: left side should be variable name" << endl;
             return this;
           }


        #ifdef DEBUG_MSG
           cout << "c0 str: " << c0->str() << " c1: " << c1->str() << endl;
        #endif
           //env.insert({c0->str(), c1});
           #if 0
           env[c0->str()] = c1;
           if (env.count(c0->str()))
           {
             ASTNode *f = env[c0->str()];
             cout << "f str: " << f->str() << endl;
           }
           #endif
           string s{c0->str()};

           #ifdef DEBUG_MSG
           cout << "s: " << s << endl;
           #endif
           ASTNode *f = env->lookup(c0->str());
           if (f)
           {
             env->edit(c0->str(), c1);
           }
           else
           {
             cout << "cannot look up: " << c0->str() << endl;
             exit(3);
           }



#if 0
           if (children_[0] != c1)
           {
             delete children_[0];
             children_[0] = c1;
             print_ast();
           }
#endif

#if 0
           if (children_[1] != c1)
           {
             delete children_[1];
             children_[1] = c1;
             print_ast();
           }
#endif
           return this;

           //free_children();
           // delete this;
           // how to free myself
         }
         else if ((str() == "root"))
              {
                ASTNode *n=0;
                cout << " op: root" << endl;
                //cout << " i child: " << i->children()[0]->str() << endl;
                for (auto &i : children_)
                {
                  n = i->eval(env);
                  #if 0
                  cout << "xx n: " << n->str() << endl;
                  //delete i;
                  //  i = n;
                  #endif
                }

                //cout << " yy op: " << str() << endl;
                //children_ = new_children;
                //print_ast();
                return this;

              }
              else if ((str() == "prog"))
                   {
                     ASTNode *n=0;
                     
                     for (auto &i : children_)
                     {
                       n = i->eval(env);
                       //cout << " op: " << str() << ", op child: " << i->str() << ", eval op child: " << n->str() << endl;
                       #if 0
                       if (n != i)
                       {
                         delete i; // need free ASTNode
                         i = n;
                       }
                       #endif
                       //print_ast();
                       //new_children.push_back(n);
                     }
                     //children_ = new_children;
                     return this;
                   }
                   else if ((str() == "if"))
                        {
                          ASTNode *then_node = 0;
                          ASTNode *else_node = 0;
                          auto children_size = children().size();

                          if (children_size >= 2)
                          {
                            ASTNode *c0 = children_[0]->eval(env);
                            if (c0->ast_type() == TRUE)
                            {
                              then_node = children_[1]->eval(env);
                              return then_node;
                            }
                            else
                            {
                              if (children_size >= 3)
                              {
                                else_node = children_[2]->eval(env);
                                return else_node;
                              }
                            }


                          }
                          else
                          {
                            cout << "if node error" << endl;
                            return this;
                          }
                        }
                        else if ((str() == ">") || (str() == "<") || (str() == "==") || (str() == "!="))
                             {
                               if (children().size() == 2)
                               {
                                 ASTNode *c0 = children_[0]->eval(env);
                                 ASTNode *c1 = children_[1]->eval(env);
                                 int n0 = stoi(c0->str());
                                 int n1 = stoi(c1->str());
                                 #ifdef DEBUG_MSG
                                 cout << endl << "n0: " << n0 << " " << str() << " n1: " << n1 << endl;
                                 #endif
                                 if (str() == ">")
                                 {
                                   if (n0 > n1)
                                   {
                                     //cout << "true node" << endl;
                                     return new ASTNode(true_token);
                                   }
                                   else
                                   {
                                     //cout << "false node" << endl;
                                     return new ASTNode(false_token);
                                   }
                                 }
                                 if (str() == "<")
                                 {
                                   if (n0 < n1)
                                   {
                                     //cout << "true node" << endl;
                                     return new ASTNode(true_token);
                                   }
                                   else
                                   {
                                     //cout << "false node" << endl;
                                     return new ASTNode(false_token);
                                   }
                                 }
                                 if (str() == "==")
                                 {
                                   if (n0 == n1)
                                   {
                                     //cout << "true node" << endl;
                                     return new ASTNode(true_token);
                                   }
                                   else
                                   {
                                     //cout << "false node" << endl;
                                     return new ASTNode(false_token);
                                   }
                                 }
                                 if (str() == "!=")
                                 {
                                   if (n0 == n1)
                                   {
                                     //cout << "true node" << endl;
                                     return new ASTNode(true_token);
                                   }
                                   else
                                   {
                                     //cout << "false node" << endl;
                                     return new ASTNode(false_token);
                                   }
                                 }


                               }
                               else
                               {
                                 cout << "> node error: " << children().size() << endl;
                                 return this;
                               }
                             }
                             else if ((str() == "then_block") || (str() == "while_block"))
                                  {

                                    ASTNode *n=0;
                                     
                                    for (auto &i : children_)
                                    {
                                      n = i->eval(env);
                                      #ifdef DEBUG_MSG
                                      cout << " op: " << str() << ", op child: " << i->str() << ", eval op child: " << n->str() << endl;
                                      #endif
                                      #if 0
                                      if (n != i)
                                      {
                                        delete i; // need free ASTNode
                                        i = n;
                                      }
                                      #endif
                                      //print_ast();
                                    }
                                    return this;
                                  }
                                  else if ((str() == "else_block"))
                                       {
                                        ASTNode *n=0;
                                       
                                        for (auto &i : children_)
                                        {
                                          n = i->eval(env);

                                          #ifdef DEBUG_MSG
                                          cout << " op: " << str() << ", op child: " << i->str() << ", eval op child: " << n->str() << endl;
                                          #endif
                                          if (n != i)
                                          {
                                            delete i; // need free ASTNode
                                            i = n;
                                          }
                                          //print_ast();
                                        }
                                        return this;
                                       }
                                       else if ((str() == "while"))
                                            {
                                              //cout << "begin eval while" << endl;
                                              if (children().size() == 2)
                                              {
                                                ASTNode *c0 = children_[0];
                                                ASTNode *eval_while = 0;
                                                while(c0->eval(env)->ast_type() == TRUE)
                                                {
                                                  eval_while = children_[1]->eval(env);
                                                  //cout << "eval while eval_while:" << eval_while->str() << endl;
                                                }
                                                return eval_while;
                                              }
                                              else
                                              {
                                                cout << "while node fail" << endl;
                                              }
                                              return this;
                                            }

                                       else
                                       {
                                         cout << "can not handle op: " << str() << ", ast type: " << type_str() << endl;
                                         return this;
                                       }
#if 0
      bool leaf = i->is_leaf();
      //i->eval();
      if (leaf == false)
        print_ast();
#endif
    } // end for (const auto &i : children_)
  }
}

#if 0
ASTNode::ASTNode(const ASTNode* l, const ASTNode* op, const ASTNode* r)
{
    vector<ASTNode*> children_;
}
#endif

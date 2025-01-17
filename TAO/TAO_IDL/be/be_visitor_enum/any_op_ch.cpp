
//=============================================================================
/**
 *  @file    any_op_ch.cpp
 *
 *  Visitor generating code for Enums for the Any operators
 *
 *  @author Aniruddha Gokhale
 */
//=============================================================================

#include "enum.h"

be_visitor_enum_any_op_ch::be_visitor_enum_any_op_ch (
    be_visitor_context *ctx)
  : be_visitor_scope (ctx)
{
}

int
be_visitor_enum_any_op_ch::visit_enum (be_enum *node)
{
  if (node->cli_hdr_any_op_gen () || node->imported ())
    {
      return 0;
    }

  TAO_OutStream *os = this->ctx_->stream ();
  const char *macro = this->ctx_->export_macro ();

  TAO_INSERT_COMMENT (os);

  be_module *module = nullptr;

  if (node->is_nested ())
    {
      AST_Decl *d = node;
      AST_Decl::NodeType nt = d->node_type ();

      while (nt != AST_Decl::NT_root)
        {
          if (nt == AST_Decl::NT_module)
            {
              module = dynamic_cast<be_module*> (d);
              break;
            }
          else
            {
              d = ScopeAsDecl (d->defined_in ());
              nt = d->node_type ();
            }
        }

      if (module != nullptr)
        {
          // Some compilers handle "any" operators in a namespace
          // corresponding to their module, others do not.
          *os << "\n\n#if defined (ACE_ANY_OPS_USE_NAMESPACE)\n";

          be_util::gen_nested_namespace_begin (os, module);

          // Generate the Any <<= and >>= operators.
          *os << be_nl_2
              << macro << " void operator<<= (::CORBA::Any &, ::" << node->name ()
              << ");" << be_nl;
          *os << macro << " ::CORBA::Boolean operator>>= (const ::CORBA::Any &, ::"
              << node->name () << " &);";

          be_util::gen_nested_namespace_end (os, module);

          // Emit #else.
          *os << be_nl_2
              << "#else\n\n";
        }
    }

  *os << be_global->core_versioning_begin () << be_nl;

  // Generate the Any <<= and >>= operators.
  *os << be_nl_2
      << macro << " void operator<<= (::CORBA::Any &, " << node->name ()
      << ");" << be_nl;
  *os << macro << " ::CORBA::Boolean operator>>= (const ::CORBA::Any &, "
      << node->name () << " &);";

  *os << be_global->core_versioning_end () << be_nl;

  if (module != nullptr)
    {
      *os << "\n\n#endif";
    }

  node->cli_hdr_any_op_gen (true);
  return 0;
}

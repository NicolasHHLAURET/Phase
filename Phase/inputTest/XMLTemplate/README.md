This folder contains templates of the XML read by the Phase module.

These templates contains the list of all possible parameters available in an XML.
v5 is the old format. v6 is the new format replacing the old format. Retro-compatibility is not managed at this level. At the end of the refactoriation, only v6 should be read.

## Mapping v5 -> v6
'coeff_diff.xml' in the v5 becomes 'materials.xml' in the v6

## ClassCatalog
ClassCatalog.xml contains the type of the attributes defined in the XML
For example <Property name="albedo" type="double" /> simply means the attribute 'albedo' in the XML is a 'double'.
Non standard type are: booleanButton->bool, inputfile->string, directionSetLink->int, opticalLink->int, enumerate->enum with the type defined in arg="...", database->string

## Template.xml
Template.xml contains the structure of the XML.
The 'DartDocumentTemplateNode' are conditional node that are dropped when creating the final XML. They provide information on the Nodes they contain.
 - Nodes directly below a 'DartDocumentTemplateNode' that contains type="list" can exist in multiple copy. In other cases -no preceding DartDocumentTemplateNode or no type="list"-, the node is unique.
 - Nodes below a 'DartDocumentTemplateNode' that contains test="_test_" exists only if _test_ is validated. 
For example: <NodeA><DartDocumentTemplateNode type="list"><NodeB d="0"><DartDocumentTemplateNode test="parent.d==0"><NodeC></DartDocumentTemplateNode></NodeB></DartDocumentTemplateNode></NodeA>
can generate for example the following XML
<NodeA>
<NodeB d="1"/>
<NodeB d="0">
<NodeC>
</NodeB>
<NodeB d="1"/>
</NodeA>
The above example has 3 NodeB, but it can be anything from 0 to infinite (or more exactly max_int). NodeC is unique per NodeB. NodeA is unique.

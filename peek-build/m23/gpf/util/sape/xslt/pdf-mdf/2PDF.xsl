<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:redirect="org.apache.xalan.xslt.extensions.Redirect" extension-element-prefixes="redirect ext-pack" xmlns:ext-pack="xalan://com.ti.xslt.extension" xmlns:exslt="http://exslt.org/common">
    <xsl:import href="../format.xsl"/>
    <xsl:import href="../utilities.xsl"/>
    <xsl:import href="../getExternType.xsl"/>
    <xsl:import href="../primID.xsl"/>
    <xsl:import href="../func-com.xsl"/>
    <xsl:import href="PDF-MDF-common.xsl"/>
    <xsl:output method="text"/>
    <!-- paramter to specify the root directory for sap and aim documents, will be set with command line -->
    <xsl:param name="sapPath"/>
    <xsl:param name="aimPath"/>
    <xsl:param name="incPath"/>
    <xsl:variable name="absSapPath">
        <xsl:value-of select="ext-pack:PathResolution.getAbsoluteURI($sapPath)"/>
    </xsl:variable>
    <xsl:variable name="absAimPath">
        <xsl:value-of select="ext-pack:PathResolution.getAbsoluteURI($aimPath)"/>
    </xsl:variable>
    <xsl:variable name="absIncPath">
        <xsl:value-of select="ext-pack:PathResolution.getAbsoluteURI($incPath)"/>
    </xsl:variable>
    <xsl:variable name="urlSapPath">
        <xsl:value-of select="ext-pack:PathResolution.getURL($sapPath, 'false', 'true')"/>
    </xsl:variable>
    <xsl:variable name="urlAimPath">
        <xsl:value-of select="ext-pack:PathResolution.getURL($aimPath, 'false', 'true')"/>
    </xsl:variable>
    <xsl:template match="/">
        <xsl:apply-templates select="/SAP/DocInfoSection">
            <xsl:with-param name="output" select="'PDF'"/>
        </xsl:apply-templates>
        <xsl:value-of select="$intersection"/>
        <xsl:call-template name="fileStatPragmas"/>
        <xsl:apply-templates select="/SAP/PragmasSection/Pragma"/>
        <xsl:value-of select="$intersection"/>
        <xsl:apply-templates select="/SAP/ConstantsSection/Constant"/>
        <xsl:value-of select="$intersection"/>
        <xsl:apply-templates select="/SAP/ValuesSection/Values">
            <xsl:with-param name="output" select="'PDF'"/>
        </xsl:apply-templates>
        <xsl:value-of select="$intersection"/>
        <xsl:apply-templates select="/SAP/PrimBasicElementsSection/PrimBasicElem"/>
        <xsl:value-of select="$intersection"/>
        <xsl:call-template name="sortedPrimStructElements"/>
        <xsl:value-of select="$intersection"/>
        <xsl:variable name="primIDs">
            <xsl:call-template name="getPrimIDs"/>
        </xsl:variable>
        <xsl:apply-templates select="/SAP/PrimitivesSection/Primitive" mode="declaration">
            <xsl:with-param name="primIDs" select="$primIDs"/>
        </xsl:apply-templates>
        <xsl:value-of select="$intersection"/>
	<!-- Recursively build list of primitives -->
        <xsl:for-each select="/SAP/PrimitivesSection/Primitive[1]">
            <xsl:call-template name="Primitives">
                <xsl:with-param name="primIDs" select="$primIDs"/>
                <xsl:with-param name="sduWritten" select="count(/*/PrimStructElementsSection/PrimStructElem/PrimStructElemDef[Name='sdu']) &gt; 0"/>
            </xsl:call-template>
        </xsl:for-each>
        <xsl:value-of select="$intersection"/>
        <xsl:apply-templates select="/SAP/SubstitutesSection/Substitute"/>
        <xsl:apply-templates select="/SAP/FunctionsSection"/>
    </xsl:template>
    <!--PrimBasicElem template: for all PrimBasicElem elements of the PrimBasicElementsSection, assemble PDF representation-->
    <xsl:template match="PrimBasicElem">
        <xsl:apply-templates select="./PrimBasicElemDef"/>
        <xsl:value-of select="$newline"/>
        <xsl:for-each select="./ValuesLink">
            <xsl:apply-templates select="."/>
            <xsl:value-of select="$newline"/>
        </xsl:for-each>
        <xsl:value-of select="$newline"/>
    </xsl:template>
    <!--PrimBasicElemDef template: check for a ENUM type, otherwise use VAR keyword. Rest of the definition is simple and straightforward. Type of the element has to be transformed into PDF representation-->
    <xsl:template match="PrimBasicElemDef">
        <xsl:choose>
            <xsl:when test='./Type="ENUM"'>
                <xsl:text>ENUM</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>VAR</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:value-of select="$delimiter"/>
        <xsl:value-of select="./Name"/>
        <xsl:apply-templates select="./Version"/>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>"</xsl:text>
        <xsl:call-template name="stripComment">
            <xsl:with-param name="str" select="./Comment"/>
        </xsl:call-template>
        <xsl:text>"</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:call-template name="transformBasicType">
            <xsl:with-param name="skipEnum" select="'yes'"/>
        </xsl:call-template>
        <xsl:value-of select="$newline"/>
        <xsl:for-each select="./Group">
            <xsl:call-template name="setGroup"/>
        </xsl:for-each>
    </xsl:template>
    <!--named template to sort structured elements so that they do not contain forward references-->
    <xsl:template name="sortedPrimStructElements">
        <xsl:variable name="sortHandle">
            <xsl:value-of select="ext-pack:ForwardRefSort.startSort()"/>
        </xsl:variable>
        <xsl:for-each select="/SAP/PrimStructElementsSection/PrimStructElem">
            <xsl:value-of select="ext-pack:ForwardRefSort.addElement($sortHandle)"/>
            <xsl:for-each select="./PrimStructElemDef">
                <xsl:value-of select="ext-pack:ForwardRefSort.addElementName($sortHandle, ./Name)"/>
            </xsl:for-each>
            <xsl:for-each select="./PrimStructElemItem">
                <xsl:choose>
                    <xsl:when test="./ItemLink and ./ItemLink/DocName = /SAP/DocInfoSection/DocName">
                        <xsl:value-of select="ext-pack:ForwardRefSort.addLinkName($sortHandle, ./ItemLink/Name)"/>
                    </xsl:when>
                </xsl:choose>
            </xsl:for-each>
        </xsl:for-each>
        <xsl:call-template name="putSortedPrimStructElements">
            <xsl:with-param name="sortHandle" select="$sortHandle"/>
        </xsl:call-template>
        <xsl:value-of select="ext-pack:ForwardRefSort.endSort($sortHandle)"/>
    </xsl:template>
    <!--named template to output sorted structured elements-->
    <xsl:template name="putSortedPrimStructElements">
        <xsl:param name="sortHandle"/>
        <!-- Call limited sub recursion. -->
        <xsl:call-template name="putSortedPrimStructElements100">
            <xsl:with-param name="sortHandle" select="$sortHandle"/>
        </xsl:call-template>
        <xsl:if test="ext-pack:ForwardRefSort.hasNextForwardRefResolved($sortHandle) &gt; 0">
            <!-- Call main recursion. -->
            <xsl:call-template name="putSortedPrimStructElements">
                <xsl:with-param name="sortHandle" select="$sortHandle"/>
            </xsl:call-template>
        </xsl:if>
    </xsl:template>
    <xsl:template name="putSortedPrimStructElements100">
        <xsl:param name="sortHandle"/>
        <xsl:param name="recursionCount" select="0"/>
        <xsl:choose>
            <xsl:when test="$recursionCount &gt; 99">
                <!-- stop recursion here, goto next chunk manually in order to not end up in a stack overflow.-->
            </xsl:when>
            <xsl:otherwise>
                <xsl:variable name="actElemIndex">
                    <xsl:value-of select="number(ext-pack:ForwardRefSort.getNextForwardRefResolved($sortHandle) + 1)"/>
                </xsl:variable>
                <xsl:choose>
                    <xsl:when test="$actElemIndex > 0">
                        <xsl:apply-templates select="/SAP/PrimStructElementsSection/PrimStructElem[$actElemIndex]"/>
                        <xsl:call-template name="putSortedPrimStructElements100">
                            <xsl:with-param name="sortHandle" select="$sortHandle"/>
                            <xsl:with-param name="recursionCount" select="number($recursionCount + 1)"/>
                        </xsl:call-template>
                    </xsl:when>
                    <xsl:when test="$actElemIndex = 0">
                        <!-- No more elements, end recursion. -->
                    </xsl:when>
                    <xsl:when test="$actElemIndex = -1">
                        <!--unknown sort handle-->
                        <xsl:message terminate="yes">
                            <xsl:call-template name="cmdLineMsg">
                                <xsl:with-param name="msgType" select="error"/>
                                <xsl:with-param name="msg">
                                    <xsl:text>Internal error when sorting structured elements: invalid sort handle!</xsl:text>
                                </xsl:with-param>
                            </xsl:call-template>
                        </xsl:message>
                    </xsl:when>
                    <xsl:when test="$actElemIndex = -2">
                        <!--circular dependencies-->
                        <xsl:message terminate="no">
                            <xsl:call-template name="cmdLineMsg">
                                <xsl:with-param name="msgType" select="error"/>
                                <xsl:with-param name="msg">
                                    <xsl:text>Circular dependency between structured elements, starting with element "</xsl:text>
                                    <xsl:value-of select="ext-pack:ForwardRefSort.getCircularInfo($sortHandle)"/>
                                    <xsl:text>"!!!</xsl:text>
                                </xsl:with-param>
                            </xsl:call-template>
                        </xsl:message>
                        <!--Write down the unsorted list-->
                        <xsl:apply-templates select="/SAP/PrimStructElementsSection/PrimStructElem"/>
                        <!-- End recursion. -->
                    </xsl:when>
                    <xsl:otherwise>
                        <!--unknown error type-->
                        <xsl:message terminate="yes">
                            <xsl:call-template name="cmdLineMsg">
                                <xsl:with-param name="msgType" select="error"/>
                                <xsl:with-param name="msg">
                                    <xsl:text>Internal error when sorting structured elements.</xsl:text>
                                </xsl:with-param>
                            </xsl:call-template>
                        </xsl:message>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <!--PrimStructElem template: for all PrimStructElem elements of the PrimStructElementsSection, assemble PDF representation. First PrimStructElemDef will be simple and straight forward. All PrimStructElemItem will be processed. In case that more than one PrimStructElemDef is present and the type is the same of the first definition, a TYPEDEF construct will assembled.-->
    <xsl:template match="PrimStructElem">
        <xsl:apply-templates select="./PrimStructElemDef[1]"/>
        <xsl:value-of select="$openbracket"/>
        <xsl:apply-templates select="./PrimStructElemItem"/>
        <xsl:value-of select="$closebracket"/>
        <xsl:choose>
            <xsl:when test="count(./PrimStructElemDef) &gt; 1">
                <xsl:for-each select="./PrimStructElemDef">
                    <xsl:choose>
                        <xsl:when test="position() = 1">
                            <!--do nothing here, element 1 is already handled !-->
                        </xsl:when>
                        <xsl:when test="(position() != 1) and ./@Type = ../PrimStructElemDef[1]/@Type">
                            <xsl:text>TYPEDEF </xsl:text>
                            <xsl:choose>
                                <xsl:when test='./@Type = "UNION"'>
                                    <xsl:text>UNION </xsl:text>
                                </xsl:when>
                                <xsl:otherwise>
                                    <xsl:text>COMP </xsl:text>
                                </xsl:otherwise>
                            </xsl:choose>
                            <xsl:value-of select="../PrimStructElemDef[1]/Name"/>
                            <xsl:value-of select="$delimiter"/>
                            <xsl:value-of select="./Name"/>
                            <xsl:value-of select="$delimiter"/>
                            <xsl:choose>
                                <xsl:when test="./Alias">
                                    <xsl:text> AS </xsl:text>
                                    <xsl:value-of select="./Alias"/>
                                </xsl:when>
                            </xsl:choose>
                            <xsl:text> "</xsl:text>
                            <xsl:call-template name="stripComment">
                                <xsl:with-param name="str" select="./Comment"/>
                            </xsl:call-template>
                            <xsl:text>"</xsl:text>
                            <xsl:value-of select="$newline"/>
                            <xsl:for-each select="./Group">
                                <xsl:call-template name="setGroup"/>
                            </xsl:for-each>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:apply-templates select="."/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:for-each>
            </xsl:when>
        </xsl:choose>
        <xsl:value-of select="$intersection"/>
    </xsl:template>
    <!--PrimStructElemDef template: check for a UNION type, otherwise use COMP keyword. Rest of the definition is simple and straightforward.-->
    <xsl:template match="PrimStructElemDef">
        <!-- Call of common template is not possible because of the "Alias" element: xsl:call-template name="StructElemDef"/-->
        <xsl:choose>
            <xsl:when test='./@Type="UNION"'>
                <xsl:text>UNION</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>COMP</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:value-of select="$delimiter"/>
        <xsl:value-of select="./Name"/>
        <xsl:value-of select="$delimiter"/>
        <xsl:choose>
            <xsl:when test="./Alias">
                <xsl:text> AS </xsl:text>
                <xsl:value-of select="./Alias"/>
            </xsl:when>
        </xsl:choose>
        <xsl:apply-templates select="./Version"/>
        <xsl:text> "</xsl:text>
        <xsl:call-template name="stripComment">
            <xsl:with-param name="str" select="./Comment"/>
        </xsl:call-template>
        <xsl:text>"</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:for-each select="./Group">
            <xsl:call-template name="setGroup"/>
        </xsl:for-each>
    </xsl:template>
    <!--PrimStructElemItem template: check for a UNION type and UnionTag element, to place UnionTag in front of element Name. Rest of the definition is simple and straightforward.-->
    <xsl:template match="PrimStructElemItem">
        <xsl:value-of select="$delimiter"/>
        <xsl:call-template name="optElemBegin"/>
        <xsl:if test='./UnionTag and "UNION" = ../PrimStructElemDef[1]/@Type'>
            <xsl:value-of select="./UnionTag/Name"/>
            <xsl:if test="./UnionTag/Value">
                <xsl:text>=</xsl:text>
                <xsl:call-template name="getValPrefix">
                    <xsl:with-param name="valType" select="./UnionTag/Value/@ValueType"/>
                </xsl:call-template>
                <xsl:value-of select="./UnionTag/Value"/>
            </xsl:if>
            <xsl:value-of select="$delimiter"/>
        </xsl:if>
        <xsl:call-template name="StructElemItem"/>
    </xsl:template>
    <!--Primitive template, mode of declaration: Transformation is simple and straight forward.-->
    <xsl:template match="Primitive" mode="declaration">
        <xsl:param name="primIDs"/>
        <xsl:variable name="actPos" select="position()"/>
        <xsl:text>; </xsl:text>
        <xsl:value-of select="./PrimDef/Name"/>
        <xsl:value-of select="$delimiter"/>
        <xsl:for-each select="exslt:node-set($primIDs)/prim[$actPos]/id">
            <xsl:value-of select="."/>
        </xsl:for-each>
        <xsl:value-of select="$newline"/>
    </xsl:template>
    <!--Primitive template, mode of definition: -->
    <xsl:template name="Primitives">
        <xsl:param name="primIDs"/>
        <xsl:param name="sduWritten" select="false()"/>
        <xsl:param name="actPos" select="1"/>
        <xsl:variable name="memHandleElement">
            <xsl:value-of select="exslt:node-set($primIDs)/prim[$actPos]/memHandle"/>
        </xsl:variable>
        <xsl:choose>
            <!--Test if this primitive is linked to some other primitive-->
            <xsl:when test="count(./ItemLink) &gt; 0">
                <!-- Find out about if linked primitive has a mem-handle. -->
                <xsl:variable name="memHandleElement">
                    <xsl:variable name="extFileName">
                        <xsl:call-template name="getDocumentName">
                            <xsl:with-param name="docName" select="./ItemLink/DocName"/>
                            <xsl:with-param name="docType" select="./ItemLink/DocName/@DocType"/>
                        </xsl:call-template>
                    </xsl:variable>
                    <xsl:variable name="elName" select="./ItemLink/Name"/>
                    <xsl:for-each select="document($extFileName)/SAP/PrimitivesSection/Primitive[PrimDef/Name=$elName]">
                        <xsl:call-template name="findMemHandles"/>
                    </xsl:for-each>
                </xsl:variable>
                <!-- Linked primitive: create typedefs-->
                <xsl:for-each select="./PrimDef">
                    <xsl:text>TYPEDEF PRIM </xsl:text>
                    <xsl:apply-templates select="../ItemLink" mode="typedef"/>
                    <xsl:value-of select="./Name"/>
                    <xsl:value-of select="$delimiter"/>                    
                    <xsl:choose>
                        <xsl:when test="$memHandleElement &gt; 0">
                            <xsl:apply-templates select="./PrimID">
                                <xsl:with-param name="memHandle" select="true()"/>
                            </xsl:apply-templates>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:apply-templates select="./PrimID"/>
                        </xsl:otherwise>
                    </xsl:choose>
                    <xsl:value-of select="$delimiter"/>
                    <xsl:apply-templates select="./Version"/>
                    <xsl:value-of select="$newline"/>
                    <xsl:for-each select="./Group">
                        <xsl:call-template name="setGroup"/>
                    </xsl:for-each>
                </xsl:for-each>
            </xsl:when>
            <xsl:otherwise>
                <!-- Regular primitive: create definition and item list-->
                <xsl:for-each select="./PrimDef[1]">
                    <xsl:if test="$memHandleElement &gt; 0 and $sduWritten=false()">
                        <xsl:call-template name="sdu_element"/>
                    </xsl:if>
                    <xsl:text>PRIM</xsl:text>
                    <xsl:value-of select="$delimiter"/>
                    <xsl:value-of select="./Name"/>
                    <xsl:value-of select="$delimiter"/>
                    <xsl:value-of select="exslt:node-set($primIDs)/prim[$actPos]/id[1]"/>
                    <xsl:apply-templates select="./Version"/>
                    <xsl:value-of select="$newline"/>
                    <xsl:for-each select="./Group">
                        <xsl:call-template name="setGroup"/>
                    </xsl:for-each>
                    <xsl:value-of select="$openbracket"/>
                    <xsl:if test="$memHandleElement &gt; 0">
                        <!-- Write mem-handle element. -->
                        <xsl:for-each select="../PrimItem">
                            <xsl:if test="$memHandleElement=position()">
                                <xsl:call-template name="PrimItem">
                                    <xsl:with-param name="memHandleElement" select="$memHandleElement"/>
                                </xsl:call-template>
                            </xsl:if>
                        </xsl:for-each>
                    </xsl:if>
                        <!-- Write non mem-handle elements. -->
                    <xsl:for-each select="../PrimItem">
                        <xsl:if test="$memHandleElement!=position()">
                            <xsl:call-template name="PrimItem"/>
                        </xsl:if>
                    </xsl:for-each>
                    <xsl:value-of select="$closebracket"/>
                    <xsl:value-of select="$intersection"/>
                </xsl:for-each>
                <xsl:choose>
                    <xsl:when test="count(./PrimDef) &gt; 1">
                        <xsl:for-each select="./PrimDef">
                            <xsl:variable name="defPos" select="position()"/>
                            <xsl:choose>
                                <xsl:when test="position() = 1">
                                    <!--do nothing here, element 1 is already handled !-->
                                </xsl:when>
                                <xsl:when test="(position() != 1)">
                                    <xsl:text>TYPEDEF PRIM </xsl:text>
                                    <xsl:value-of select="../PrimDef[1]/Name"/>
                                    <xsl:value-of select="$delimiter"/>
                                    <xsl:value-of select="./Name"/>
                                    <xsl:value-of select="$delimiter"/>
                                    <xsl:value-of select="exslt:node-set($primIDs)/prim[$actPos]/id[$defPos]"/>
                                    <xsl:value-of select="$delimiter"/>
                                    <xsl:apply-templates select="./Version"/>
                                    <xsl:value-of select="$newline"/>
                                    <xsl:for-each select="./Group">
                                        <xsl:call-template name="setGroup"/>
                                    </xsl:for-each>
                                </xsl:when>
                                <xsl:otherwise>
                                    <xsl:apply-templates select="."/>
                                </xsl:otherwise>
                            </xsl:choose>
                        </xsl:for-each>
                    </xsl:when>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:value-of select="$intersection"/>
        <!-- Recurse down to next primitive. -->
        <xsl:if test="following-sibling::Primitive">
            <xsl:for-each select="following-sibling::Primitive[1]">
                <xsl:call-template name="Primitives">
                    <xsl:with-param name="primIDs" select="$primIDs"/>
                    <xsl:with-param name="sduWritten" select="($memHandleElement &gt; 0) or $sduWritten"/>
                    <xsl:with-param name="actPos" select="$actPos+1"/>
                </xsl:call-template>
            </xsl:for-each>
        </xsl:if>
    </xsl:template>
    <!--Recursive template which returns the position of the first item that is a mem-handle. -->
    <xsl:template name="findMemHandles">
        <xsl:param name="pos" select="1"/>
        <xsl:choose>
            <xsl:when test="$pos &lt;= count(./PrimItem)">
                <xsl:variable name="itemType">
                    <xsl:for-each select="./PrimItem[$pos]">
                        <xsl:if test='./ItemLink/DocName[@DocType = "SAP"]'>
                            <xsl:choose>
                                <xsl:when test='./ItemLink/DocName = /SAP/DocInfoSection/DocName'>
                                    <xsl:apply-templates select="/SAP" mode="getTypeForName">
                                        <xsl:with-param name="elemName" select="./ItemLink/Name"/>
                                        <xsl:with-param name="transformType" select="'compOnly'"/>
                                    </xsl:apply-templates>
                                </xsl:when>
                                <xsl:otherwise>
                                    <xsl:call-template name="getExternType">
                                        <xsl:with-param name="elemName" select="./ItemLink/Name"/>
                                        <xsl:with-param name="docType" select="./ItemLink/DocName/@DocType"/>
                                        <xsl:with-param name="docName" select="./ItemLink/DocName"/>
                                        <xsl:with-param name="transformType" select="'compOnly'"/>
                                    </xsl:call-template>
                                </xsl:otherwise>
                            </xsl:choose>
                        </xsl:if>
                    </xsl:for-each>
                </xsl:variable>
                <xsl:choose>
                    <!-- Does PrimItem link to mem-handle element? -->
                    <xsl:when test="$itemType='MEMHANDLE'">
                        <xsl:value-of select="$pos"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:call-template name="findMemHandles">
                            <xsl:with-param name="pos" select="$pos+1"/>
                        </xsl:call-template>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:when>
            <xsl:otherwise>
                <!-- Noting found -->
                <xsl:text>0</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <xsl:template name="getPrimIDs">
        <xsl:for-each select="/*/PrimitivesSection/Primitive">
            <xsl:variable name="memHandleElement">
                <xsl:call-template name="findMemHandles"/>
            </xsl:variable>
            <prim>
                <xsl:for-each select="./PrimDef">
                    <id>
                        <xsl:choose>
                            <xsl:when test="$memHandleElement &gt; 0">
                                <xsl:apply-templates select="./PrimID">
                                    <xsl:with-param name="memHandle" select="true()"/>
                                </xsl:apply-templates>
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:apply-templates select="./PrimID"/>
                            </xsl:otherwise>
                        </xsl:choose>
                    </id>
                </xsl:for-each>
                <memHandle>
                    <xsl:value-of select="$memHandleElement"/>
                </memHandle>
            </prim>
        </xsl:for-each>
    </xsl:template>
    <xsl:template name="sdu_element">
        <!-- Manually add sdu struct and its members. -->
        <xsl:text>VAR</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>l_buf</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>"length of content in bit"</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>S</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:text>VAR</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>o_buf</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>"offset of content in bit"</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>S</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:text>VAR</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>buf</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>"memhandle data"</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>B</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:value-of select="$newline"/>
        <xsl:text>COMP</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>sdu</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>"memhandle data"</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:text>{</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>l_buf</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text> ; length of content in bit</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>o_buf</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text> ; offset of content in bit</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>buf</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>[1]</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:text> ; memhandle data</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:text>}</xsl:text>
        <xsl:value-of select="$newline"/>
        <xsl:value-of select="$newline"/>
    </xsl:template>
    <!--PrimItem template: Transformation is simple and straight forward.-->
    <xsl:template name="PrimItem">
        <xsl:param name="memHandleElement" select="0"/>
        <xsl:value-of select="$delimiter"/>
        <xsl:call-template name="optElemBegin"/>
        <xsl:call-template name="StructElemItem"/>
        <!-- Add SDU PTR for memhandle element. -->
        <xsl:if test="$memHandleElement=position()">
            <xsl:value-of select="$delimiter"/>
            <xsl:text>&lt;</xsl:text>
            <xsl:value-of select="$delimiter"/>
            <xsl:text>sdu</xsl:text>
            <xsl:value-of select="$delimiter"/>
            <xsl:text>PTR</xsl:text>
            <xsl:value-of select="$delimiter"/>
            <xsl:text>&gt;</xsl:text>
            <xsl:value-of select="$delimiter"/>
            <xsl:text> ; memhandle target buffer</xsl:text>
            <xsl:value-of select="$newline"/>                
        </xsl:if>
    </xsl:template>
    <!--FunctionsSection template:This template opens a new output file if this feature is supported by the XSLT processor. Otherwise the processing of the stylesheet terminates with a message. In case a new output file can be opened, the template scans for all Function Elements and assembles the corresponding C function prototype. All links to internal or external function arguments have to be resolved, to build the prototype.-->
    <xsl:template match="FunctionsSection">
        <xsl:variable name="docName" select="/SAP/DocInfoSection/DocName"/>
        <!-- Generate PDF entries for functions if desired. -->
        <xsl:for-each select="./Function[FuncDef/@IsLinkTarget='YES']">
            <xsl:text>TYPEDEF FUNC </xsl:text>
            <xsl:value-of select="./FuncDef/Name"/>
            <xsl:value-of select="$delimiter"/>
            <xsl:if test="./FuncDef/Alias">
                <xsl:text>AS </xsl:text>
                <xsl:value-of select="./FuncDef/Alias"/>
                <xsl:value-of select="$delimiter"/>
            </xsl:if>

            <!-- Return value, empty if none. -->
            <xsl:text>(</xsl:text>
            <xsl:if test="./FuncRet">
                <xsl:choose>
                    <xsl:when test="./FuncRet/ExtType">
                        <!-- ERROR! Only types defined in an SAP/AIM document are allowed as return types for functions that can be used as link target. -->
                        <xsl:message terminate="yes">
                            <xsl:call-template name="cmdLineMsg">
                                <xsl:with-param name="msgType" select="error"/>
                                <xsl:with-param name="msg">
                                    <xsl:text>Error in function prototype "</xsl:text>
                                    <xsl:value-of select="./FuncDef/Name"/>
                                    <xsl:text>": only types defined in SAP/AIM documents are allowed as return types for functions that can be used as link target.</xsl:text>
                                </xsl:with-param>
                            </xsl:call-template>
                        </xsl:message>                        
                    </xsl:when>
                    <xsl:otherwise>            
                        <xsl:apply-templates select="./FuncRet/ItemLink"/>
                        <xsl:value-of select="./FuncRet/ItemLink/Name"/>
                        <xsl:if test="./FuncRet/Control and string-length(./FuncRet/Control) &gt; 0">
                            <xsl:value-of select="$delimiter"/>
                            <xsl:value-of select="./FuncRet/Control"/>
                        </xsl:if>
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:if>
            <xsl:text>)</xsl:text>
            <xsl:for-each select="./FuncArg">
                <xsl:text>(</xsl:text>
                <xsl:choose>
                    <xsl:when test="./ExtType">
                        <!-- ERROR! Only types defined in an SAP/AIM document are allowed as argument types for functions that can be used as link target. -->
                        <xsl:message terminate="yes">
                            <xsl:call-template name="cmdLineMsg">
                                <xsl:with-param name="msgType" select="error"/>
                                <xsl:with-param name="msg">
                                    <xsl:text>Error in function prototype "</xsl:text>
                                    <xsl:value-of select="../FuncDef/Name"/>
                                    <xsl:text>": only types defined in SAP/AIM documents are allowed as argument types for functions that can be used as link target.</xsl:text>
                                </xsl:with-param>
                            </xsl:call-template>
                        </xsl:message>                        
                        </xsl:when>
                        <xsl:otherwise>                        
                            <xsl:apply-templates select="./ItemLink"/>
                            <xsl:choose>
                                <xsl:when test="./Alias">
                                    <xsl:value-of select="./ItemLink/Name"/>
                                    <xsl:text> AS </xsl:text>
                                    <xsl:value-of select="./Alias"/>
                                </xsl:when>
                                <xsl:otherwise>
                                    <xsl:value-of select="./ItemLink/Name"/>
                                </xsl:otherwise>
                            </xsl:choose>
                            <xsl:if test="./Control and string-length(./Control) &gt; 0">
                                <xsl:value-of select="$delimiter"/>
                                <xsl:value-of select="./Control"/>
                            </xsl:if>
                        </xsl:otherwise>
                    </xsl:choose>
                <xsl:text>)</xsl:text>
            </xsl:for-each>            
            <xsl:apply-templates select="./FuncDef/Version"/>
            <xsl:for-each select="./FuncDef/Group">
                <xsl:value-of select="$newline"/>
                <xsl:call-template name="setGroup"/>
            </xsl:for-each>
            <xsl:value-of select="$intersection"/>
        </xsl:for-each>
        <!-- Generate file containing function declarations. -->
        <xsl:choose>
            <xsl:when test="contains( system-property('xsl:vendor-url'), 'xalan')">
                <!--extension function for Xalan to open an additional output file, in this case it is the C include file for this SAP-->
                <redirect:write select="concat( $absIncPath, $docName, '_inline.h' )">
                    <xsl:for-each select="/SAP/DocInfoSection">
                        <xsl:apply-templates select=".">
                            <xsl:with-param name="output" select="'C'"/>
                        </xsl:apply-templates>
                    </xsl:for-each>
                    <xsl:text>#ifndef _</xsl:text>
                    <xsl:call-template name="toUpper">
                        <xsl:with-param name="str" select="$docName"/>
                    </xsl:call-template>
                    <xsl:text>_INLINE_H_</xsl:text>
                    <xsl:value-of select="$newline"/>
                    <xsl:text>#define _</xsl:text>
                    <xsl:call-template name="toUpper">
                        <xsl:with-param name="str" select="$docName"/>
                    </xsl:call-template>
                    <xsl:text>_INLINE_H_</xsl:text>
                    <xsl:value-of select="$newline"/>
                    <xsl:value-of select="$newline"/>                    
                    <xsl:variable name="includes">
                        <xsl:for-each select="./Function/*/ExtType/ExtSource">
                            <xsl:sort/>
                            <include>
                                <xsl:value-of select="."/>
                            </include>
                        </xsl:for-each>
                    </xsl:variable>
                    <xsl:for-each select="exslt:node-set($includes)/include">
                        <xsl:if test="not(preceding-sibling::include[1]=.)">
                            <xsl:text>#include "</xsl:text>
                            <xsl:value-of select="."/>
                            <xsl:text>"</xsl:text>
                            <xsl:value-of select="$newline"/>
                        </xsl:if>
                    </xsl:for-each>
                    <xsl:value-of select="$newline"/>
                    <xsl:value-of select="$newline"/>
                    <xsl:for-each select="./Function">
                        <xsl:if test="./FuncDef/Version and string-length(./FuncDef/Version) &gt; 0">
                            <xsl:text>#if </xsl:text>
							<!-- TODO parse || and && operators -->
							<xsl:choose>
								<xsl:when test="starts-with(./FuncDef/Version,'!')">
									<xsl:text>!defined </xsl:text>
									<xsl:value-of select="substring(./FuncDef/Version,2)"/>
								</xsl:when>
								<xsl:otherwise>
									<xsl:text>defined </xsl:text>
									<xsl:value-of select="./FuncDef/Version"/>
								</xsl:otherwise>
							</xsl:choose>
                            <xsl:value-of select="$newline"/>
                        </xsl:if>
                        <xsl:text>extern </xsl:text>
                        <xsl:value-of select="$space"/>
                        <xsl:call-template name="writeFunctionDeclaration"/>
                        <xsl:value-of select="$newline"/>
                        <xsl:if test="./FuncDef/Version and string-length(./FuncDef/Version) &gt; 0">
                            <xsl:text>#endif /* of #if </xsl:text>
                            <xsl:value-of select="./FuncDef/Version"/>
                            <xsl:text> */</xsl:text>
                            <xsl:value-of select="$newline"/>
                        </xsl:if>
                        <xsl:value-of select="$newline"/>
                    </xsl:for-each>
                    <xsl:value-of select="$intersection"/>
                    <xsl:text>#endif /* !_</xsl:text>
                    <xsl:call-template name="toUpper">
                        <xsl:with-param name="str" select="$docName"/>
                    </xsl:call-template>
                    <xsl:text>_INLINE_H_ */</xsl:text>
                    <xsl:value-of select="$newline"/>
                </redirect:write>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">
                    <xsl:call-template name="cmdLineMsg">
                        <xsl:with-param name="msgType" select="error"/>
                        <xsl:with-param name="msg">
                            <xsl:text>the choosen XSLT processor does not support the creation of multiple file, or the stylesheet does not know how to use it. The C include file with function prototypes could not be generated.</xsl:text>
                        </xsl:with-param>
                    </xsl:call-template>
                </xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!--named template to transform the common part for items of structured elements.-->
    <xsl:template name="StructElemItem">
        <xsl:apply-templates select="./ItemLink"/>
        <xsl:choose>
            <xsl:when test="./Alias">
                <xsl:value-of select="./ItemLink/Name"/>
                <xsl:text> AS </xsl:text>
                <xsl:value-of select="./Alias"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="./ItemLink/Name"/>
            </xsl:otherwise>
        </xsl:choose>
        <xsl:value-of select="$delimiter"/>
        <xsl:if test="./Control and string-length(./Control) &gt; 0">
            <xsl:value-of select="./Control"/>
            <xsl:value-of select="$delimiter"/>
        </xsl:if>
        <xsl:call-template name="optElemEnd"/>
        <xsl:apply-templates select="./Version"/>
        <xsl:text> ; </xsl:text>
        <xsl:call-template name="stripComment">
            <xsl:with-param name="str" select="./Comment"/>
        </xsl:call-template>
        <xsl:value-of select="$newline"/>
    </xsl:template>
    
    <!--named template to indictate the beginning of an optional element-->
    <xsl:template name="optElemBegin">
        <xsl:if test='self::node()[@Presentation="OPTIONAL"] or self::node()[@Presentation="CONDITIONAL"]'>
            <xsl:text>&lt; ()</xsl:text>
            <xsl:value-of select="$delimiter"/>
        </xsl:if>
    </xsl:template>
    <!--named template to indictate the ending of an optional element-->
    <xsl:template name="optElemEnd">
        <xsl:if test='self::node()[@Presentation="OPTIONAL"] or self::node()[@Presentation="CONDITIONAL"]'>
            <xsl:text>&gt;</xsl:text>
            <xsl:value-of select="$delimiter"/>
        </xsl:if>
    </xsl:template>
    <!--named template to get the Comment for a linked element which resides within the same document-->
    <xsl:template name="getLinkComment">
        <xsl:if test='./ItemLink/DocName = /SAP/DocInfoSection/DocName and ./ItemLink/DocName[@DocType="SAP"]'>
            <xsl:variable name="itemLinkName">
                <xsl:value-of select="./ItemLink/Name"/>
            </xsl:variable>
            <xsl:choose>
                <xsl:when test="/SAP/PrimStructElementsSection/PrimStructElem/PrimStructElemDef/Name = $itemLinkName">
                    <xsl:for-each select="/SAP/PrimStructElementsSection/PrimStructElem/PrimStructElemDef">
                        <xsl:if test="./Name = $itemLinkName">
                            <xsl:call-template name="stripComment">
                                <xsl:with-param name="str" select="./Comment"/>
                            </xsl:call-template>
                        </xsl:if>
                    </xsl:for-each>
                </xsl:when>
                <xsl:when test="/SAP/PrimBasicElementsSection/PrimBasicElem/PrimBasicElemDef/Name = $itemLinkName">
                    <xsl:for-each select="/SAP/PrimBasicElementsSection/PrimBasicElem/PrimBasicElemDef">
                        <xsl:if test="./Name = $itemLinkName">
                            <xsl:call-template name="stripComment">
                                <xsl:with-param name="str" select="./Comment"/>
                            </xsl:call-template>
                        </xsl:if>
                    </xsl:for-each>
                </xsl:when>
            </xsl:choose>
        </xsl:if>
    </xsl:template>
    <xsl:template name="test">
        <xsl:for-each select='document("MNXX_INC.sap")/SAP/PrimBasicElementsSection/PrimBasicElem/PrimBasicElemDef[Type="U8"]'>
            <xsl:text> Count = </xsl:text>
            <xsl:value-of select='count(document("MNXX_INC.sap")/SAP/PrimBasicElementsSection/PrimBasicElem/PrimBasicElemDef[Type="U8"])'/>
            <xsl:text> Name = </xsl:text>
            <xsl:value-of select="./Name"/>
            <xsl:text> Type = </xsl:text>
            <xsl:value-of select="./Type"/>
            <xsl:value-of select="$newline"/>
        </xsl:for-each>
    </xsl:template>
</xsl:stylesheet>

<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:ext-pack="xalan://com.ti.xslt.extension" extension-element-prefixes="ext-pack">
    <xsl:import href="../format.xsl"/>
    <xsl:import href="../utilities.xsl"/>
    <xsl:import href="PDF-MDF-common.xsl"/>
    <xsl:output method="text"/>
    <!-- paramter to specify the root directory for sap and aim documents, will be set with command line -->
    <xsl:param name="sapPath"/>
    <xsl:param name="aimPath"/>
    <xsl:param name="incPath"/>
	<!-- asn1-fix
		0: no fix
		1: print range after valType ref to avoid ccdgen crash when calulating per bit size
		2: eleminate valType to avoid ccdgen generating BUF types for ASN1_INTEGER > 24bit
	-->
	<xsl:param name="asn1-fix" select="0"/>
    <xsl:variable name="absSapPath">
        <xsl:value-of select="ext-pack:PathResolution.getAbsoluteURI($sapPath)"/>
    </xsl:variable>
    <xsl:variable name="absAimPath">
        <xsl:value-of select="ext-pack:PathResolution.getAbsoluteURI($aimPath)"/>
    </xsl:variable>
    <xsl:variable name="urlSapPath">
        <xsl:value-of select="ext-pack:PathResolution.getURL($sapPath, 'false', 'true')"/>
    </xsl:variable>
    <xsl:variable name="urlAimPath">
        <xsl:value-of select="ext-pack:PathResolution.getURL($aimPath, 'false', 'true')"/>
    </xsl:variable>

    <xsl:template match="/">
        <xsl:apply-templates select="/AIM/DocInfoSection">
            <xsl:with-param name="output" select="'MDF'"/>
        </xsl:apply-templates>
        <xsl:value-of select="$intersection"/>
        <xsl:call-template name="fileStatPragmas"/>
        <xsl:apply-templates select="/AIM/PragmasSection/Pragma"/>
        <xsl:value-of select="$intersection"/>
        <xsl:apply-templates select="/AIM/ConstantsSection/Constant"/>
        <xsl:value-of select="$intersection"/>
        <xsl:apply-templates select="/AIM/ValuesSection/Values">
            <xsl:with-param name="output" select="'MDF'"/>
        </xsl:apply-templates>
        <xsl:value-of select="$intersection"/>
        <xsl:apply-templates select="/AIM/MsgBasicElementsSection/MsgBasicElem"/>
        <xsl:value-of select="$intersection"/>
        <xsl:call-template name="sortedMsgStructElements"/>
        <xsl:value-of select="$intersection"/>
        <xsl:apply-templates select="/AIM/MessagesSection/Message"/>
        <xsl:value-of select="$intersection"/>
        <xsl:apply-templates select="/AIM/SubstitutesSection/Substitute"/>
    </xsl:template>
    <!--MsgBasicElem template: for all MsgBasicElem elements of the MsgBasicElementsSection, assemble MDF representation-->
	<xsl:key name="Values" match="//Values" use="ValuesDef/Name"/>
    <xsl:template match="MsgBasicElem">
        <xsl:apply-templates select="./MsgBasicElemDef"/>
        <xsl:value-of select="$newline"/>
		<xsl:if test="$asn1-fix!=2">
			<xsl:for-each select="./ValuesLink">
				<xsl:apply-templates select="."/>
				<xsl:value-of select="$newline"/>
			</xsl:for-each>
		</xsl:if>
		<xsl:if test="$asn1-fix=1">
			<xsl:for-each select="key('Values',./ValuesLink/Name)/ValuesRange">
				<xsl:text>RANGE</xsl:text>
				<xsl:value-of select="$delimiter"/>
				<xsl:call-template name="getValPrefix">
					<xsl:with-param name="valType" select="./@ValueType"/>
				</xsl:call-template>
				<xsl:value-of select="./MinValue"/>
				<xsl:text> .. </xsl:text>
				<xsl:call-template name="getValPrefix">
					<xsl:with-param name="valType" select="./@ValueType"/>
				</xsl:call-template>
				<xsl:value-of select="./MaxValue"/>
			</xsl:for-each>
		</xsl:if>
		<xsl:if test="$asn1-fix=2">
			<xsl:apply-templates select="key('Values',./ValuesLink/Name)" mode="Values-elements">
				<xsl:with-param name="output" select="'MDF'"/>
			</xsl:apply-templates>
		</xsl:if>
        <xsl:value-of select="$newline"/>
    </xsl:template>
    <!--MsgBasicElemDef template: Definition is simple and straightforward. Bit length of element will be placed in next line-->
    <xsl:template match="MsgBasicElemDef">
        <xsl:text>VAR</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:value-of select="./Name"/>
        <xsl:if test="./Alias">
            <xsl:value-of select="$delimiter"/>
            <xsl:text>AS </xsl:text>
            <xsl:value-of select="./Alias"/>
        </xsl:if>
        <xsl:apply-templates select="./Version"/>
        <xsl:value-of select="$delimiter"/>
        <xsl:text>"</xsl:text>
        <xsl:call-template name="stripComment">
            <xsl:with-param name="str" select="./Comment"/>
        </xsl:call-template>
        <xsl:text>"</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:value-of select="$newline"/>
        <xsl:value-of select="$delimiter"/>
        <xsl:call-template name="getBitLen"/>
        <xsl:value-of select="$newline"/>
        <xsl:for-each select="./Group">
            <xsl:call-template name="setGroup"/>
        </xsl:for-each>
    </xsl:template>    
    <xsl:template name="sortedMsgStructElements">
        <xsl:variable name="sortHandle">
            <xsl:value-of select="ext-pack:ForwardRefSort.startSort()"/>
        </xsl:variable>
        <xsl:for-each select="/AIM/MsgStructElementsSection/MsgStructElem">
            <xsl:value-of select="ext-pack:ForwardRefSort.addElement($sortHandle)"/>
            <xsl:for-each select="./MsgStructElemDef">
                <xsl:value-of select="ext-pack:ForwardRefSort.addElementName($sortHandle, ./Name)"/>
            </xsl:for-each>
            <xsl:for-each select="./MsgStructElemItem">
                <xsl:choose>
                    <xsl:when test="./ItemLink and ./ItemLink/DocName = /AIM/DocInfoSection/DocName">
                        <xsl:value-of select="ext-pack:ForwardRefSort.addLinkName($sortHandle, ./ItemLink/Name)"/>
                    </xsl:when>
                </xsl:choose>
            </xsl:for-each>
        </xsl:for-each>
        <xsl:call-template name="putSortedMsgStructElements">
            <xsl:with-param name="sortHandle" select="$sortHandle"/>
        </xsl:call-template>
        <xsl:value-of select="ext-pack:ForwardRefSort.endSort($sortHandle)"/>
    </xsl:template>    
    <!--named template to output sorted structured elements-->
    <xsl:template name="putSortedMsgStructElements">
        <xsl:param name="sortHandle"/>
        <!-- Call limited sub recursion. -->
        <xsl:call-template name="putSortedMsgStructElements100">
            <xsl:with-param name="sortHandle" select="$sortHandle"/>
        </xsl:call-template>
        <xsl:if test="ext-pack:ForwardRefSort.hasNextForwardRefResolved($sortHandle) &gt; 0">
            <!-- Call main recursion. -->
            <xsl:call-template name="putSortedMsgStructElements">
                <xsl:with-param name="sortHandle" select="$sortHandle"/>
            </xsl:call-template>
        </xsl:if>
    </xsl:template>
    <xsl:template name="putSortedMsgStructElements100">
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
                        <xsl:apply-templates select="/AIM/MsgStructElementsSection/MsgStructElem[$actElemIndex]"/>
                        <xsl:call-template name="putSortedMsgStructElements100">
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
                        <xsl:apply-templates select="/AIM/MsgStructElementsSection/MsgStructElem"/>
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
    <!--MsgStructElem template: for all MsgStructElem elements of the MsgStructElementsSection, assemble MDF representation. First MsgStructElemDef will be simple and straight forward. All MsgStructElemItem will be processed. In case that more than one MsgStructElemDef is present and the type is the same of the first definition, a TYPEDEF construct will assembled.-->
    <xsl:template match="MsgStructElem">
        <xsl:apply-templates select="./MsgStructElemDef[1]"/>
        <xsl:value-of select="$openbracket"/>
        <xsl:apply-templates select="./MsgStructElemItem"/>
        <xsl:value-of select="$closebracket"/>
        <xsl:choose>
            <xsl:when test="count(./MsgStructElemDef) &gt; 1">
                <xsl:for-each select="./MsgStructElemDef">
                    <xsl:choose>
                        <xsl:when test="position() = 1">
                            <!--do nothing here, element 1 is already handled !-->
                        </xsl:when>
                        <xsl:when test="(position() != 1) and ./@Type = ../MsgStructElemDef[1]/@Type">
                            <xsl:text>TYPEDEF </xsl:text>
                            <xsl:choose>
                                <xsl:when test='./@Type = "UNION"'>
                                    <xsl:text>UNION </xsl:text>
                                </xsl:when>
                                <xsl:otherwise>
                                    <xsl:text>COMP </xsl:text>
                                </xsl:otherwise>
                            </xsl:choose>
                            <xsl:value-of select="../MsgStructElemDef[1]/Name"/>
                            <xsl:value-of select="$delimiter"/>
                            <xsl:value-of select="./Name"/>
                            <xsl:value-of select="$delimiter"/>
                            <xsl:text>"</xsl:text>
                            <xsl:call-template name="stripComment">
                                <xsl:with-param name="str" select="./Comment"/>
                            </xsl:call-template>
                            <xsl:text>"</xsl:text>
                            <xsl:value-of select="$newline"/>
                            <xsl:for-each select="./Group">
                                <xsl:call-template name="setGroup"/>
                            </xsl:for-each>
                        </xsl:when>
                    </xsl:choose>
                </xsl:for-each>
            </xsl:when>
        </xsl:choose>
        <xsl:value-of select="$intersection"/>
    </xsl:template>
    <!--MsgStructElemDef template: check for a UNION type, otherwise use COMP keyword. Rest of the definition is simple and straightforward.-->
    <xsl:template match="MsgStructElemDef">
        <xsl:call-template name="StructElemDef"/>
    </xsl:template>
    <!--MsgStructElemItem template: call template for common transformation of structured element items-->
    <xsl:template match="MsgStructElemItem">
        <xsl:call-template name="StructElemItem"/>
    </xsl:template>
    <!--named template to transform the declration of an element item. Distinguish between a Spare and an Item Link. Otherwise the transformation is straight forward.-->
    <xsl:template name="StructElemItem">
        <xsl:value-of select="$delimiter"/>
        <xsl:call-template name="optElemBegin"/>
        <xsl:if test="./Control/BitGroupDef">
            <xsl:value-of select="./Control/BitGroupDef"/>
            <xsl:value-of select="$delimiter"/>
        </xsl:if>
        <xsl:choose>
            <xsl:when test="./Spare">
                <xsl:if test="./Type">
                    <xsl:value-of select="./Type"/>
                    <xsl:value-of select="$space"/>
                </xsl:if>
                <xsl:text>.</xsl:text>
                <xsl:value-of select="./Spare/Pattern"/>
                <xsl:value-of select="$space"/>
            </xsl:when>
            <xsl:when test="./ItemLink">
                <xsl:if test='./UnionTag and "UNION" = ../MsgStructElemDef[1]/@Type'>
                    <xsl:value-of select="./UnionTag/Name"/>
                    <xsl:if test="./UnionTag/Value">
                        <xsl:text>=</xsl:text>
                        <xsl:call-template name="getValPrefix">
                            <xsl:with-param name="valType" select="./UnionTag/Value/@ValueType"/>
                        </xsl:call-template>
                        <xsl:value-of select="./UnionTag/Value"/>
                    </xsl:if>
                    <xsl:value-of select="$space"/>
                </xsl:if>
                <xsl:if test="./Type">
                    <xsl:value-of select="./Type"/>
                    <xsl:value-of select="$space"/>
                </xsl:if>
                <xsl:apply-templates select="./ItemLink"/>
                <xsl:value-of select="$delimiter"/>
                <xsl:value-of select="./ItemLink/Name"/>
                <xsl:value-of select="$delimiter"/>
                <xsl:if test="./Alias">
                    <xsl:text>AS </xsl:text>
                    <xsl:value-of select="./Alias"/>
                    <xsl:value-of select="$delimiter"/>
                </xsl:if>
            </xsl:when>
        </xsl:choose>
        <xsl:if test="./Control/TypeModifier">
            <xsl:choose>
                <!--special treatment for bitfield arrays: The upper boundary will be defined by the bit length of the element itself !-->
                <xsl:when test="contains(./Control/TypeModifier,'[.') and (./Type = 'GSM4_TLV' or ./Type = 'GSM5_TLV' or ./Type = 'GSM6_TLV' or ./Type = 'GSM1_ASN' or ./Type = 'GSM_LV' or ./Type = 'GSM_TV')">
                    <xsl:choose>
                        <!--for variable bit fields:-->
                        <xsl:when test="contains(./Control/TypeModifier,'..')">
                            <xsl:value-of select="substring-before(./Control/TypeModifier,'..')"/>
                            <xsl:text>..</xsl:text>
                            <xsl:apply-templates select="document(concat($urlAimPath, ./ItemLink/DocName, '.aim'))/AIM" mode="getBitLenForName">
                                <xsl:with-param name="elemName" select="./ItemLink/Name"/>
                                <xsl:with-param name="default" select="substring-before(substring-after(./Control/TypeModifier,'..'),']')"/>
                            </xsl:apply-templates>
                            <xsl:text>]</xsl:text>
                            <xsl:value-of select="substring-after(./Control/TypeModifier,']')"/>
                        </xsl:when>
                        <!--for constant bit fields:-->
                        <xsl:otherwise>
                            <xsl:value-of select="substring-before(./Control/TypeModifier,'[')"/>
                            <xsl:text>[.</xsl:text>
                            <xsl:apply-templates select="document(concat($urlAimPath, ./ItemLink/DocName, '.aim'))/AIM" mode="getBitLenForName">
                                <xsl:with-param name="elemName" select="./ItemLink/Name"/>
                                <xsl:with-param name="default" select="substring-before(substring-after(./Control/TypeModifier,'.'),']')"/>
                            </xsl:apply-templates>
                            <xsl:text>]</xsl:text>
                            <xsl:value-of select="substring-after(./Control/TypeModifier,']')"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="./Control/TypeModifier"/>
                </xsl:otherwise>
            </xsl:choose>
            <xsl:value-of select="$delimiter"/>
        </xsl:if>
        <xsl:if test="./ItemTag">
            <xsl:call-template name="getValPrefix">
                <xsl:with-param name="valType" select="./ItemTag/@TagType"/>
            </xsl:call-template>
            <xsl:value-of select="./ItemTag"/>
            <xsl:value-of select="$delimiter"/>
        </xsl:if>
        <xsl:if test="./Control/CmdSequence">
            <xsl:text>(</xsl:text>
            <xsl:value-of select="./Control/CmdSequence"/>
            <xsl:text>)</xsl:text>
        </xsl:if>
        <xsl:call-template name="optElemEnd"/>
        <xsl:apply-templates select="./Version"/>
        <xsl:text> ; </xsl:text>
        <xsl:call-template name="stripComment">
            <xsl:with-param name="str" select="./Comment"/>
        </xsl:call-template>
        <xsl:value-of select="$newline"/>
    </xsl:template>
    <!--Message template: for all Message elements of the MessagesSection, assemble MDF representation. -->
    <xsl:template match="Message">
        <xsl:for-each select="./MsgDef">
            <xsl:apply-templates select="."/>
            <xsl:value-of select="$openbracket"/>
            <xsl:apply-templates select="../MsgItem"/>
            <xsl:value-of select="$closebracket"/>
            <xsl:value-of select="$intersection"/>
        </xsl:for-each>
    </xsl:template>
    <!--MsgDef template: The transformation is simple and straight forward.-->
    <xsl:template match="MsgDef">
        <xsl:text>MSG</xsl:text>
        <xsl:value-of select="$delimiter"/>
        <xsl:value-of select="./Name"/>
        <xsl:value-of select="$delimiter"/>
        <xsl:call-template name="toLower">
            <xsl:with-param name="str" select="./MsgID/@Direction"/>
        </xsl:call-template>
        <xsl:value-of select="$delimiter"/>
        <xsl:call-template name="getValPrefix">
            <xsl:with-param name="valType" select="./MsgID/@IDType"/>
        </xsl:call-template>
        <xsl:value-of select="./MsgID"/>
        <xsl:apply-templates select="./Version"/>
        <xsl:text> ; </xsl:text>
        <xsl:call-template name="stripComment">
            <xsl:with-param name="str" select="./Comment"/>
        </xsl:call-template>
        <xsl:value-of select="$newline"/>
        <xsl:for-each select="./Group">
            <xsl:call-template name="setGroup"/>
        </xsl:for-each>
    </xsl:template>
    <!--MsgItem template: call template for common transformation of structured element items-->
    <xsl:template match="MsgItem">
        <xsl:call-template name="StructElemItem"/>
    </xsl:template>
    <!--AIM template, mode of getBitLenForName:This template scans the node list for a basic element, which matches the elemName parameter. Once the element is found, the bit length for that element  will be outputed-->
    <xsl:template match="AIM" mode="getBitLenForName">
        <xsl:param name="elemName"/>
        <xsl:param name="default"/>
        <xsl:choose>
            <xsl:when test="count(./MsgBasicElementsSection/MsgBasicElem/MsgBasicElemDef[Name=$elemName]) = 1">
                <xsl:for-each select="./MsgBasicElementsSection/MsgBasicElem/MsgBasicElemDef[Name=$elemName]">
                    <xsl:call-template name="getBitLen"/>
                </xsl:for-each>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="$default"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <!--named template to calculate the bit length of an element-->
    <xsl:template name="getBitLen">
        <xsl:choose>
            <xsl:when test="./ByteLen">
                <xsl:value-of select="./ByteLen * 8"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="./BitLen"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <!--named template to indictate the beginning of an optional element-->
    <xsl:template name="optElemBegin">
        <xsl:choose>
            <xsl:when test="./Control/Condition">
                <xsl:text>&lt; (</xsl:text>
                <xsl:value-of select="./Control/Condition"/>
                <xsl:text>)</xsl:text>
                <xsl:value-of select="$delimiter"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:if test='self::node()[@Presentation="OPTIONAL"] or self::node()[@Presentation="CONDITIONAL"]'>
                    <xsl:if test='self::node()[Type="ASN1_INTEGER"] or self::node()[Type="ASN1_SEQUENCE"] or self::node()[Type="ASN1_CHOICE"] or self::node()[Type="ASN1_OCTET"] or self::node()[Type="ASN1_INTEGER_EXTENSIBLE"] or self::node()[Type="ASN1_SEQUENCE_EXTENSIBLE"] or self::node()[Type="ASN1_CHOICE_EXTENSIBLE"] or self::node()[Type="ASN1_OBJ_ID"] or self::node()[Type="ASN1_OPEN_TYPE"] or self::node()[Type="BITSTRING"]'>
                        <xsl:text>&lt; ()</xsl:text>
                        <xsl:value-of select="$delimiter"/>
                    </xsl:if>
                </xsl:if>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <!--named template to indictate the ending of an optional element-->
    <xsl:template name="optElemEnd">
        <xsl:choose>
            <xsl:when test="./Control/Condition">
                <xsl:text>&gt;</xsl:text>
                <xsl:value-of select="$delimiter"/>            
            </xsl:when>
            <xsl:otherwise>
                <xsl:if test='self::node()[@Presentation="OPTIONAL"] or self::node()[@Presentation="CONDITIONAL"]'>
                    <xsl:if test='self::node()[Type="ASN1_INTEGER"] or self::node()[Type="ASN1_SEQUENCE"] or self::node()[Type="ASN1_CHOICE"] or self::node()[Type="ASN1_OCTET"] or self::node()[Type="ASN1_INTEGER_EXTENSIBLE"] or self::node()[Type="ASN1_SEQUENCE_EXTENSIBLE"] or self::node()[Type="ASN1_CHOICE_EXTENSIBLE"] or self::node()[Type="ASN1_OBJ_ID"] or self::node()[Type="ASN1_OPEN_TYPE"] or self::node()[Type="BITSTRING"]'>
                        <xsl:text>&gt;</xsl:text>
                        <xsl:value-of select="$delimiter"/>
                    </xsl:if>
                </xsl:if>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>        
</xsl:stylesheet>

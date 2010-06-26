<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:redirect="org.apache.xalan.xslt.extensions.Redirect" extension-element-prefixes="redirect">
    <xsl:output method="text"/>
    
    <!--SAP template, mode of getTypeForName:This template scans the node list for a basic or structured element, which fits the elemName parameter. Once the element is found, the transformed type will be outputed-->
    <xsl:template match="SAP" mode="getTypeForName">
        <xsl:param name="elemName"/>
        <xsl:param name="transformType" select="'yes'"/>
        <xsl:variable name="cntBsc">
            <xsl:value-of select="count(./PrimBasicElementsSection/PrimBasicElem/PrimBasicElemDef[Name=$elemName])"/>
        </xsl:variable>
        <xsl:variable name="cntStr">
            <xsl:value-of select="count(./PrimStructElementsSection/PrimStructElem/PrimStructElemDef[Name=$elemName])"/>
        </xsl:variable>
        <xsl:if test="($cntBsc + $cntStr) &gt; 1">
            <xsl:message terminate="no">
                <xsl:call-template name="cmdLineMsg">
                    <xsl:with-param name="msgType" select="error"/>
                    <xsl:with-param name="msg">
                        <xsl:text>more than one element with name: </xsl:text>
                        <xsl:value-of select="$elemName"/>
                        <xsl:text> exists in external file.</xsl:text>
                        <xsl:text> struct: </xsl:text>
                        <xsl:value-of select="$cntStr"/>
                        <xsl:text> basic: </xsl:text>
                        <xsl:value-of select="$cntBsc"/>
                        <xsl:text> document: </xsl:text>
                        <xsl:value-of select="/*/DocInfoSection/DocName"/>
                    </xsl:with-param>
                </xsl:call-template>
            </xsl:message>
        </xsl:if>
        <xsl:choose>
            <xsl:when test="$cntBsc = 1">
                <xsl:for-each select="./PrimBasicElementsSection/PrimBasicElem/PrimBasicElemDef[Name=$elemName]">

                    <!-- Write out type of a basic element, depending on the given context. -->
                    <xsl:choose>
                        <xsl:when test='$transformType = "yes"'>
                            <xsl:call-template name="transformBasicType"/>
                        </xsl:when>
                        <xsl:when test='$transformType = "varOnly"'>
                            <xsl:text>VAR</xsl:text>
                        </xsl:when>
                        <xsl:when test='$transformType = "compAndEnumOnly" and ./Type="ENUM"'>
                            <xsl:choose>
                                <xsl:when test="../ValuesLink">
                                    <xsl:call-template name="writeStructTypeName">
                                        <xsl:with-param name="structName" select="./Name"/>
                                    </xsl:call-template>
                                </xsl:when>
                                <xsl:otherwise>
                                    <xsl:choose>
	                                 <xsl:when test="./Type='ENUM'">
                                            <!-- Emulate behaviour of Word version here and set type U8 for (explicit) ENUM types without values because CCDGEN does not generate types for such empty enumerations. -->
                                            <xsl:text>U8</xsl:text>
	                                 </xsl:when>
                                        <xsl:otherwise>
                                            <xsl:value-of select="./Type"/>
                                        </xsl:otherwise>													
                                    </xsl:choose>
                                </xsl:otherwise>
                            </xsl:choose>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="./Type"/>
                        </xsl:otherwise>
                    </xsl:choose>
                     
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$cntStr = 1">
                <xsl:for-each select="./PrimStructElementsSection/PrimStructElem/PrimStructElemDef[Name=$elemName]">
                    <xsl:choose>
                        <xsl:when test='$transformType = "yes" or $transformType = "varOnly"'>
                            <xsl:call-template name="transformBasicType"/>
                        </xsl:when>
                        <xsl:when test='$transformType = "compOnly" or $transformType = "compAndEnumOnly"'>
                            <xsl:call-template name="writeStructTypeName">
                                <xsl:with-param name="structName">
                                    <xsl:choose>
                                        <xsl:when test="./Alias">
                                            <xsl:value-of select="./Alias"/>
                                        </xsl:when>
                                        <xsl:otherwise>
                                            <xsl:value-of select="./Name"/>
                                        </xsl:otherwise>
                                    </xsl:choose>
				</xsl:with-param>
                            </xsl:call-template>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="./@Type"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:for-each>
            </xsl:when>
        </xsl:choose>
    </xsl:template>
    
    <!--AIM template, mode of getTypeForName:This template scans the node list for a basic or structured element, which fits the elemName parameter. Once the element is found, the transformed type will be outputed-->
    <xsl:template match="AIM" mode="getTypeForName">
        <xsl:param name="elemName"/>
        <xsl:param name="transformType" select="'yes'"/>
        <xsl:variable name="cntBsc">
            <xsl:value-of select="count(./MsgBasicElementsSection/MsgBasicElem/MsgBasicElemDef[Name=$elemName])"/>
        </xsl:variable>
        <xsl:variable name="cntStr">
            <xsl:value-of select="count(./MsgStructElementsSection/MsgStructElem/MsgStructElemDef[Name=$elemName])"/>
        </xsl:variable>
        <xsl:if test="($cntBsc + $cntStr) &gt; 1">
            <xsl:message terminate="no">
                <xsl:call-template name="cmdLineMsg">
                    <xsl:with-param name="msgType" select="error"/>
                    <xsl:with-param name="msg">
                        <xsl:text>more than one element with name: </xsl:text>
                        <xsl:value-of select="$elemName"/>
                        <xsl:text> exists in external file.</xsl:text>
                    </xsl:with-param>
                </xsl:call-template>
            </xsl:message>
        </xsl:if>
        <xsl:choose>
            <xsl:when test="$cntBsc = 1">
                <xsl:for-each select="./MsgBasicElementsSection/MsgBasicElem/MsgBasicElemDef[Name=$elemName]">
                    <xsl:choose>
                        <xsl:when test='$transformType = "yes"'>
                            <xsl:call-template name="transformBasicType"/>
                        </xsl:when>
                        <xsl:when test='$transformType = "varOnly"'>
                            <xsl:text>VAR</xsl:text>
                        </xsl:when>
                        <xsl:when test='$transformType = "noIgnoreAimBasic"'/>
                        <xsl:otherwise>
                            <xsl:message terminate="no">
                                <xsl:call-template name="cmdLineMsg">
                                    <xsl:with-param name="msgType" select="error"/>
                                    <xsl:with-param name="msg">
                                        <xsl:text>ERROR: Data type of AIM basic element </xsl:text>
                                        <xsl:value-of select="./Name"/>
                                        <xsl:text> requested (by e.g. code generating function prototypes). There exists no such information for this type of element.</xsl:text>
                                    </xsl:with-param>
                                </xsl:call-template>
                            </xsl:message>
                            <xsl:value-of select="./Type"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:for-each>
            </xsl:when>
            <xsl:when test="$cntStr = 1">
                <xsl:for-each select="./MsgStructElementsSection/MsgStructElem/MsgStructElemDef[Name=$elemName]">
                    <xsl:choose>
                        <xsl:when test='$transformType = "yes" or $transformType = "varOnly"'>
                            <xsl:call-template name="transformBasicType"/>
                        </xsl:when>
                        <xsl:when test='$transformType = "compOnly" or $transformType = "compAndEnumOnly"'>
                            <xsl:call-template name="writeStructTypeName">
	                        <xsl:with-param name="structName" select="./Name"/>
                            </xsl:call-template>
                        </xsl:when>
                        <xsl:otherwise>
                            <xsl:value-of select="./@Type"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:for-each>
            </xsl:when>
        </xsl:choose>
    </xsl:template>
    
    <!--named template to transform the basic type into the PDF type representation for Type elements-->
    <xsl:template name="transformBasicType">
        <xsl:param name="skipEnum" select="'no'"/>
        <xsl:choose>
            <xsl:when test='./Type="BYTE" or ./Type="S8"'>
                <xsl:text>C</xsl:text>
            </xsl:when>
            <xsl:when test='./Type="UBYTE" or ./Type="U8"'>
                <xsl:text>B</xsl:text>
            </xsl:when>
            <xsl:when test='./Type="UWORD" or ./Type="U16"'>
                <xsl:text>S</xsl:text>
            </xsl:when>
            <xsl:when test='./Type="WORD" or ./Type="S16"'>
                <xsl:text>T</xsl:text>
            </xsl:when>
            <xsl:when test='./Type="ULONG"  or ./Type="U32"'>
                <xsl:text>L</xsl:text>
            </xsl:when>
            <xsl:when test='./Type="LONG" or ./Type="S32"'>
                <xsl:text>M</xsl:text>
            </xsl:when>
            <xsl:when test='./Type="MEMHANDLE"'>
                <xsl:text>L</xsl:text>
            </xsl:when>
            <xsl:when test='./@Type="STRUCT"'>
                <xsl:text>COMP</xsl:text>
            </xsl:when>
            <xsl:when test='./@Type="UNION"'>
                <xsl:text>UNION</xsl:text>
            </xsl:when>
            <xsl:when test='./Type="ENUM"'>
                <xsl:if test='$skipEnum="no"'>
                    <xsl:text>ENUM</xsl:text>
                </xsl:if>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">
                    <xsl:call-template name="cmdLineMsg">
                        <xsl:with-param name="msgType" select="error"/>
                        <xsl:with-param name="msg">
                            <xsl:text>Type is neither of BYTE, UBYTE, WORD, UWORD, LONG, ULONG, S8, U8, S16, U16, S32,  U32, STRUCT, ENUM, MEMHANDLE or UNION</xsl:text>
                        </xsl:with-param>
                    </xsl:call-template>
                </xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!--named template to get the type of an external element. First of all the filename will be assembled. Depending on the type of document, the node list of the document will be read.-->
    <xsl:template name="getExternType">
        <xsl:param name="elemName"/>
        <xsl:param name="docType"/>
        <xsl:param name="docName"/>
        <xsl:param name="transformType" select="'yes'"/>
        <xsl:variable name="filename">
            <xsl:call-template name="getDocumentName">
                <xsl:with-param name="docName" select="$docName"/>
                <xsl:with-param name="docType" select="$docType"/>
            </xsl:call-template>
        </xsl:variable>
        <xsl:choose>
            <xsl:when test='$docType="SAP"'>
                <xsl:apply-templates select="document($filename)/SAP" mode="getTypeForName">
                    <xsl:with-param name="elemName" select="$elemName"/>
                    <xsl:with-param name="transformType" select="$transformType"/>
                </xsl:apply-templates>
            </xsl:when>
            <xsl:when test='$docType="AIM"'>
                <xsl:apply-templates select="document($filename)/AIM" mode="getTypeForName">
                    <xsl:with-param name="elemName" select="$elemName"/>
                    <xsl:with-param name="transformType" select="$transformType"/>
                </xsl:apply-templates>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">
                    <xsl:call-template name="cmdLineMsg">
                        <xsl:with-param name="msgType" select="error"/>
                        <xsl:with-param name="msg">
                            <xsl:text>DocName@DocType is neither SAP nor AIM</xsl:text>
                        </xsl:with-param>
                    </xsl:call-template>
                </xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!-- Named template to get the name of a document element. -->
    <xsl:template name="getDocumentName">
        <xsl:param name="docName"/>
        <xsl:param name="docType"/>
        <xsl:choose>
            <xsl:when test='$docType="SAP"'>
                <xsl:value-of select="concat($urlSapPath, $docName)"/>
                <xsl:text>.</xsl:text>
                <xsl:text>sap</xsl:text>
            </xsl:when>
            <xsl:when test='$docType="AIM"'>
                <xsl:value-of select="concat($urlAimPath, $docName)"/>
                <xsl:text>.</xsl:text>
                <xsl:text>aim</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">
                    <xsl:call-template name="cmdLineMsg">
                        <xsl:with-param name="msgType" select="error"/>
                        <xsl:with-param name="msg">
                            <xsl:text>Unknown type of document </xsl:text>
                            <xsl:value-of select="$docName"/>
                            <xsl:text>: </xsl:text>
                            <xsl:value-of select="$docType"/>
                            <xsl:text>. Expected is type "SAP" or "AIM".</xsl:text>
                        </xsl:with-param>
                    </xsl:call-template>
                </xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Named template to write the type name of a document element. -->
    <xsl:template name="writeStructTypeName">
        <xsl:param name="structName"/>
        <xsl:variable name="typeName">
            <xsl:text>T_</xsl:text>
            <xsl:if test='/*/PragmasSection/Pragma[Name="PREFIX"]'>
                <xsl:for-each select='/*/PragmasSection/Pragma[Name="PREFIX"]'>
                    <xsl:variable name="prefixName">
                        <xsl:call-template name="toLower">
                            <xsl:with-param name="str" select="./Value"/>
                        </xsl:call-template>
                    </xsl:variable>
                    <xsl:if test="$prefixName != 'none'">
                        <xsl:value-of select="./Value"/>
                        <xsl:text>_</xsl:text>
                    </xsl:if>
                </xsl:for-each>
            </xsl:if>
	    <xsl:value-of select="$structName"/>
	</xsl:variable>
	<xsl:variable name="doCapitalize">
	    <xsl:call-template name="isCapitalizeTypename"/>
	</xsl:variable>
	<xsl:choose>
	    <xsl:when test="$doCapitalize='true'">
	        <xsl:call-template name="toUpper">
		    <xsl:with-param name="str" select="$typeName"/>
		</xsl:call-template>
	    </xsl:when>
	    <xsl:otherwise>
	        <xsl:value-of select="$typeName"/>
	    </xsl:otherwise>
	</xsl:choose>
    </xsl:template>

    <!-- Named template to find out if PRAGMA ALWAYS_ENUM_IN_VAL_FILE is set to 'YES'. -->
    <xsl:template name="isAlwaysEnum">
        <xsl:variable name="isAlwaysEnum">
            <xsl:call-template name="isAlwaysEnumWalker"/>
        </xsl:variable>
        <xsl:choose>
            <xsl:when test="number($isAlwaysEnum) &gt; 0">
                <xsl:text>true</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>false</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <xsl:template name="isAlwaysEnumWalker">
        <xsl:text>0</xsl:text>
        <xsl:for-each select="/*/PragmasSection/Pragma[Value='YES']">
            <xsl:if test="./Name='ALLWAYS_ENUM_IN_VAL_FILE' or ./Name='ALWAYS_ENUM_IN_VAL_FILE'">
                <xsl:text>1</xsl:text>
            </xsl:if>
        </xsl:for-each>
    </xsl:template>

    <!-- Named template to find out if PRAGMA CAPITALIZE_TYPENAME is set to 'YES'. -->
    <xsl:template name="isCapitalizeTypename">
        <xsl:variable name="isCapitalizeTypename">
            <xsl:call-template name="isCapitalizeTypenameWalker"/>
        </xsl:variable>
        <xsl:choose>
            <xsl:when test="number($isCapitalizeTypename) &gt; 0">
                <xsl:text>true</xsl:text>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>false</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    <xsl:template name="isCapitalizeTypenameWalker">
        <xsl:text>0</xsl:text>
        <xsl:for-each select="/*/PragmasSection/Pragma[Name='CAPITALIZE_TYPENAME']">
            <xsl:if test="./Value='YES'">
                <xsl:text>1</xsl:text>
            </xsl:if>
        </xsl:for-each>
    </xsl:template>

</xsl:stylesheet>

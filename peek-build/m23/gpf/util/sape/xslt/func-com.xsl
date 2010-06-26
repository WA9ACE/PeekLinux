<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="text"/>
	<!--named template to transform the Control element syntax into a similar C-like expression for pointers.-->
	<xsl:template name="getPointers">
		<xsl:param name="ctrlStr"/>
		<xsl:if test='contains($ctrlStr, "PTR")'>
			<xsl:text>*</xsl:text>
			<xsl:call-template name="getPointers">
				<xsl:with-param name="ctrlStr" select='substring-after($ctrlStr,"PTR")'/>
			</xsl:call-template>
		</xsl:if>
	</xsl:template>
    
	<!--named template to transform the Control element syntax into a similar C-like expression for arrays.-->
	<xsl:template name="getArrays">
		<xsl:param name="ctrlStr"/>
		<xsl:if test='contains($ctrlStr, "[")'>
			<xsl:text>[</xsl:text>
			<xsl:value-of select='substring-after(substring-before($ctrlStr,"]"),"[")'/>
			<xsl:text>]</xsl:text>
			<xsl:call-template name="getArrays">
				<xsl:with-param name="ctrlStr" select='substring-after($ctrlStr,"]")'/>
			</xsl:call-template>
		</xsl:if>
	</xsl:template>
    
    <!-- Named template to write a function prototype declaration as a function pointer. -->
    <xsl:template name="writeFunctionAsPointer">
        <xsl:call-template name="writeFuncRetDataType"/>
        <xsl:text>( *</xsl:text>
        <xsl:call-template name="writeFuncName"/>
        <xsl:text> )( </xsl:text>
        <xsl:call-template name="writeFuncArgList"/>
        <xsl:text> )</xsl:text>
    </xsl:template>

    <!-- Named template to write out a regular function prototype declaration. -->
    <xsl:template name="writeFunctionDeclaration">
        <xsl:call-template name="writeFuncRetDataType"/>
        <xsl:call-template name="writeFuncName"/>
        <xsl:text> ( </xsl:text>
        <xsl:call-template name="writeFuncArgList"/>
        <xsl:text> );</xsl:text>
    </xsl:template>
    
    <!-- Named template to write out the name of a function prototype declaration. -->
    <xsl:template name="writeFuncName">
        <xsl:choose>
            <xsl:when test="./FuncDef/Alias">
                <xsl:value-of select="./FuncDef/Alias"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:if test='/SAP/PragmasSection/Pragma[Name="PREFIX"]'>
                    <xsl:for-each select='/SAP/PragmasSection/Pragma[Name="PREFIX"]'>
                        <xsl:variable name="prefixName">
                            <xsl:call-template name="toLower">
                                <xsl:with-param name="str" select="./Value"/>
                            </xsl:call-template>
                        </xsl:variable>
                        <xsl:if test="$prefixName != 'none'">
                            <xsl:value-of select="$prefixName"/>
                            <xsl:text>_</xsl:text>
                        </xsl:if>
                    </xsl:for-each>
                </xsl:if>
                <xsl:value-of select="./FuncDef/Name"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Named template to write out the data type of the return value of a function prototype declaration. -->
    <xsl:template name="writeFuncRetDataType">
        <xsl:choose>
            <xsl:when test="./FuncRet">
                <xsl:choose>
                    <xsl:when test="./FuncRet/ExtType">
                        <xsl:value-of select="./FuncRet/ExtType/Type"/>
                    </xsl:when>
                    <xsl:otherwise>
                        <xsl:for-each select="./FuncRet[1]/ItemLink[1]">
                            <xsl:call-template name="writeItemLinkFuncType"/>
                        </xsl:for-each>
                    </xsl:otherwise>
                </xsl:choose>
                <xsl:value-of select="$space"/>
                <xsl:call-template name="getPointers">
                    <xsl:with-param name="ctrlStr" select="./FuncRet/Control"/>
                </xsl:call-template>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>void</xsl:text>
                <xsl:value-of select="$space"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
    
    <!-- Named template to write out the comma separated list of the arguments of a function prototype declaration. -->
    <xsl:template name="writeFuncArgList">
        <xsl:choose>
            <xsl:when test="count( ./FuncArg )  &gt; 0 ">
                <xsl:variable name="docName" select="/SAP/DocInfoSection/DocName"/>
                <xsl:for-each select="./FuncArg">
                    <xsl:if test="position()  &gt; 1">
                        <xsl:text>, </xsl:text>
                    </xsl:if>
                    <xsl:variable name="linkName" select="./ItemLink/Name"/>
                    <!-- Check if this element is a function pointer. -->
                    <!-- 0: no, 1: local, 2: extern -->
                    <xsl:variable name="isFuncPtr" >
                        <xsl:choose>
                            <xsl:when test="not(./ExtType)">
                                <xsl:choose>
                                    <xsl:when test='./ItemLink/DocName = $docName and ./ItemLink/DocName[@DocType = "SAP"]'>
                                        <xsl:variable name="numDefs" select="count(/*/FunctionsSection/Function/FuncDef[Name=$linkName])"/>
                                        <xsl:choose>
                                           <xsl:when test="$numDefs&lt;=0">
                                              <xsl:value-of select="number(0)"/>
                                           </xsl:when>
                                           <xsl:when test="$numDefs&gt;=1">
                                              <xsl:value-of select="number(1)"/>
                                           </xsl:when>
                                           <xsl:when test="$numDefs&gt;1">
                                               <xsl:message terminate="no">
                                                   <xsl:call-template name="cmdLineMsg">
                                                       <xsl:with-param name="msgType" select="error"/>
                                                       <xsl:with-param name="msg">
                                                           <xsl:text>More than one function prototype with name: </xsl:text>
                                                           <xsl:value-of select="$linkName"/>
                                                           <xsl:text> exists in referenced document.</xsl:text>
                                                       </xsl:with-param>
                                                   </xsl:call-template>
                                               </xsl:message>
                                           </xsl:when>
                                       </xsl:choose>
                                    </xsl:when>
                                    <xsl:otherwise>
                                        <xsl:variable name="filename">
                                            <xsl:call-template name="getDocumentName">
                                                <xsl:with-param name="docName" select="./ItemLink/DocName"/>
                                                <xsl:with-param name="docType" select="./ItemLink/DocName/@DocType"/>
                                            </xsl:call-template>
                                        </xsl:variable>
                                        <xsl:variable name="numDefs" select="count(document($filename)/*/FunctionsSection/Function/FuncDef[Name=$linkName])"/>
                                        <xsl:choose>
                                           <xsl:when test="$numDefs&lt;=0">
                                              <xsl:value-of select="number(0)"/>
                                           </xsl:when>
                                           <xsl:when test="$numDefs&gt;=1">
                                              <xsl:value-of select="number(2)"/>
                                           </xsl:when>
                                           <xsl:when test="$numDefs&gt;1">
                                               <xsl:message terminate="no">
                                                   <xsl:call-template name="cmdLineMsg">
                                                       <xsl:with-param name="msgType" select="error"/>
                                                       <xsl:with-param name="msg">
                                                           <xsl:text>More than one function prototype with name: </xsl:text>
                                                           <xsl:value-of select="$linkName"/>
                                                           <xsl:text> exists in referenced document.</xsl:text>
                                                       </xsl:with-param>
                                                   </xsl:call-template>
                                               </xsl:message>
                                           </xsl:when>
                                       </xsl:choose>
                                    </xsl:otherwise>
                                </xsl:choose>
                            </xsl:when>
                            <xsl:otherwise>
                                <xsl:value-of select="number(0)"/>
                            </xsl:otherwise>
                        </xsl:choose>
                    </xsl:variable>
                    <xsl:choose>
                       <xsl:when test="number($isFuncPtr)=number(1)">
                           <!-- Local function pointer. -->
                           <xsl:for-each select="/*/FunctionsSection/Function[FuncDef/Name=$linkName][1]">
                               <xsl:call-template name="writeFunctionAsPointer"/>
                           </xsl:for-each>
                       </xsl:when>
                       <xsl:when test="number($isFuncPtr)=number(2)">
                           <!-- Extern function pointer. -->
                           <xsl:variable name="filename">
                               <xsl:call-template name="getDocumentName">
                                   <xsl:with-param name="docName" select="./ItemLink/DocName"/>
                                   <xsl:with-param name="docType" select="./ItemLink/DocName/@DocType"/>
                               </xsl:call-template>
                           </xsl:variable>
                           <xsl:for-each select="document($filename)/*/FunctionsSection/Function[FuncDef/Name=$linkName][1]">
                               <xsl:call-template name="writeFunctionAsPointer"/>
                           </xsl:for-each>
                       </xsl:when>
                       <xsl:otherwise>
                            <!-- Basic or struct element. -->
                            <xsl:choose>
                                <xsl:when test="./ExtType">
                                    <xsl:value-of select="./ExtType/Type"/>
                                </xsl:when>
                                <xsl:otherwise>
                                    <xsl:for-each select="./ItemLink[1]">
                                        <xsl:call-template name="writeItemLinkFuncType"/>
                                    </xsl:for-each>
                                </xsl:otherwise>
                            </xsl:choose>
                            <xsl:value-of select="$space"/>
                            <xsl:call-template name="getPointers">
                                <xsl:with-param name="ctrlStr" select="./Control"/>
                            </xsl:call-template>
                            <xsl:choose>
                                <xsl:when test="./Alias">
                                    <xsl:value-of select="./Alias"/>
                                </xsl:when>
                                <xsl:otherwise>
                                    <xsl:value-of select="./ItemLink/Name"/>
                                </xsl:otherwise>
                            </xsl:choose>
                            <xsl:call-template name="getArrays">
                                <xsl:with-param name="ctrlStr" select="./Control"/>
                            </xsl:call-template>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:for-each>
            </xsl:when>
            <xsl:otherwise>
                <xsl:text>void</xsl:text>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!-- Named template that writes out the data type of a function sub structure from an item link. -->
    <xsl:template name="writeItemLinkFuncType">
        <xsl:variable name="docName" select="/SAP/DocInfoSection/DocName"/>
        <xsl:choose>
            <xsl:when test='./DocName = $docName and ./DocName[@DocType = "SAP"]'>
                <xsl:apply-templates select="/SAP" mode="getTypeForName">
                    <xsl:with-param name="elemName" select="./Name"/>
                    <xsl:with-param name="transformType" select="'compAndEnumOnly'"/>
                </xsl:apply-templates>
            </xsl:when>
            <xsl:otherwise>
                <xsl:call-template name="getExternType">
                    <xsl:with-param name="elemName" select="./Name"/>
                    <xsl:with-param name="docType" select="./DocName/@DocType"/>
                    <xsl:with-param name="docName" select="./DocName"/>
                    <xsl:with-param name="transformType" select="'compAndEnumOnly'"/>
                </xsl:call-template>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

</xsl:stylesheet>

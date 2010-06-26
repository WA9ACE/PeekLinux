<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:ext-pack="xalan://com.ti.xslt.extension" extension-element-prefixes="ext-pack">
	<xsl:import href="../format.xsl"/>
	<xsl:import href="../getExternType.xsl"/>
	<xsl:import href="../date.xsl"/>
	<xsl:import href="../docID.xsl"/>
	<!-- asn1-fix
		0: no fix
		1: print range after valType ref to avoid ccdgen crash when calulating per bit size
		2: eleminate valType to avoid ccdgen generating BUF types for ASN1_INTEGER > 24bit
	-->
	<xsl:param name="asn1-fix" select="0"/>
	<xsl:output method="text"/>
	<!--DocInfoSection template: A header for the output file will be created. The content is pretty much the same for all different type of output files. Some adjustments have to be made to support the comment rules for the C language. Besides that the transformantion is simple and straight forward.-->
	<xsl:template match="DocInfoSection">
		<xsl:param name="output"/>
		<xsl:if test="$output='C'">
			<xsl:text>/***</xsl:text>
			<xsl:value-of select="$newline"/>
		</xsl:if>
		<xsl:value-of select="$line"/>
		<!--print out the name of the output file-->
		<xsl:value-of select="$comment"/>
		<xsl:text>File           : </xsl:text>
		<xsl:value-of select="./DocName"/>
		<xsl:choose>
			<xsl:when test="$output='PDF'">
				<xsl:text>.pdf</xsl:text>
			</xsl:when>
			<xsl:when test="$output='MDF'">
				<xsl:text>.mdf</xsl:text>
			</xsl:when>
			<xsl:when test="$output='C'">
				<xsl:text>_inline.h</xsl:text>
			</xsl:when>
		</xsl:choose>
		<xsl:value-of select="$newline"/>
		<!--print out the date and time of creation-->
		<xsl:value-of select="$comment"/>
		<xsl:text>Creation       : </xsl:text>
		<!--use customized java class of extension packet to get date and time-->
		<xsl:value-of select="ext-pack:CurrentDate.getCurrentDate( )"/>
		<xsl:value-of select="$newline"/>
		<!--print out the vendor, url and xslt version of the used XSLT processor-->
		<xsl:value-of select="$comment"/>
		<xsl:text>XSLT Processor : </xsl:text>
		<xsl:value-of select="system-property('xsl:vendor')"/>
		<xsl:text> / </xsl:text>
		<xsl:value-of select="system-property('xsl:vendor-url')"/>
		<xsl:text> / supports XSLT-Ver: </xsl:text>
		<xsl:value-of select="system-property('xsl:version')"/>
		<xsl:value-of select="$newline"/>
		<!--print out copyright notice-->
		<xsl:value-of select="$comment"/>
		<xsl:text>Copyright      : (c) Texas Instruments AG, Berlin Germany 2002</xsl:text>
		<xsl:value-of select="$newline"/>
		<xsl:value-of select="$line"/>
		<!--print out document type-->
		<xsl:value-of select="$comment"/>
		<xsl:text>Document Type  : </xsl:text>
		<xsl:choose>
			<xsl:when test='./DocName/@DocType="SAP"'>
				<xsl:text>Service Access Point Specification</xsl:text>
			</xsl:when>
			<xsl:when test='./DocName/@DocType="AIM"'>
				<xsl:text>Air Interface Message Specification</xsl:text>
			</xsl:when>
		</xsl:choose>
		<xsl:value-of select="$newline"/>
		<!--print out document name-->
		<xsl:value-of select="$comment"/>
		<xsl:text>Document Name  : </xsl:text>
		<xsl:value-of select="./DocName"/>
		<xsl:value-of select="$newline"/>
		<!--print out document number, document date, status and author-->
		<xsl:for-each select="./DocHistory">
			<xsl:if test="position()=last()">
				<xsl:value-of select="$comment"/>
				<xsl:text>Document No.   : </xsl:text>
				<xsl:apply-templates select="." mode="DocID"/>
				<xsl:value-of select="$newline"/>
				<!--print out document date-->
				<xsl:value-of select="$comment"/>
				<xsl:text>Document Date  : </xsl:text>
				<xsl:apply-templates select="./Date"/>
				<xsl:value-of select="$newline"/>
				<!--print out document status-->
				<xsl:value-of select="$comment"/>
				<xsl:text>Document Status: </xsl:text>
				<xsl:value-of select="./DocStatus/@State"/>
				<xsl:value-of select="$newline"/>
				<!--print out author-->
				<xsl:value-of select="$comment"/>
				<xsl:text>Document Author: </xsl:text>
				<xsl:value-of select="./Author"/>
				<xsl:value-of select="$newline"/>
			</xsl:if>
		</xsl:for-each>
		<xsl:value-of select="$line"/>
		<xsl:if test="$output='C'">
			<xsl:value-of select="$comment"/>
			<xsl:text>!!! THIS INCLUDE FILE WAS GENERATED AUTOMATICALLY, DO NOT MODIFY !!!</xsl:text>
			<xsl:value-of select="$newline"/>
			<xsl:value-of select="$line"/>
			<xsl:text> ***/</xsl:text>
			<xsl:value-of select="$newline"/>
		</xsl:if>
	</xsl:template>
	<!--Pragma template: for all Pragma elements of the PragmasSection, assemble PDF representation-->
	<xsl:template match="Pragma">
		<xsl:text>PRAGMA</xsl:text>
		<xsl:value-of select="$delimiter"/>
		<xsl:value-of select="./Name"/>
		<xsl:value-of select="$delimiter"/>
		<xsl:call-template name="getValPrefix">
			<xsl:with-param name="valType" select="./Value/@ValueType"/>
		</xsl:call-template>
		<xsl:value-of select="./Value"/>
		<xsl:value-of select="$delimiter"/>
		<xsl:text>; </xsl:text>
		<xsl:call-template name="stripComment">
			<xsl:with-param name="str" select="./Comment"/>
		</xsl:call-template>
		<xsl:value-of select="$newline"/>
	</xsl:template>
	<!--Constant template: for all Constant elements of the ConstantsSection, assemble PDF representation-->
	<xsl:template match="Constant">
		<xsl:choose>
			<xsl:when test="./Value">
				<xsl:text>CONST</xsl:text>
				<xsl:value-of select="$delimiter"/>
				<xsl:value-of select="./Alias"/>
				<xsl:value-of select="$delimiter"/>
				<xsl:call-template name="getValPrefix">
					<xsl:with-param name="valType" select="./Value/@ValueType"/>
				</xsl:call-template>
				<xsl:value-of select="./Value"/>
			</xsl:when>
			<xsl:when test="./ItemLink">
				<xsl:text>EXTERN CONST</xsl:text>
				<xsl:value-of select="$delimiter"/>
				<xsl:for-each select="./ItemLink">
					<xsl:call-template name="assembleLink"/>
				</xsl:for-each>
				<xsl:value-of select="./Alias"/>
			</xsl:when>
		</xsl:choose>
		<xsl:apply-templates select="./Version"/>
		<xsl:value-of select="$delimiter"/>
		<xsl:text>; </xsl:text>
		<xsl:call-template name="stripComment">
			<xsl:with-param name="str" select="./Comment"/>
		</xsl:call-template>
		<xsl:value-of select="$newline"/>
		<xsl:for-each select="./Group">
			<xsl:call-template name="setGroup"/>
		</xsl:for-each>
	</xsl:template>
	<!--ValuesLink template:  Definition is simple and straight forward.-->
	<xsl:template match="ValuesLink">
		<xsl:text>VAL</xsl:text>
		<xsl:value-of select="$delimiter"/>
		<xsl:call-template name="assembleLink"/>
	</xsl:template>
	<!--Values template: First line will be the declaration of the values table with keyword VALTAB and Name. In case a range is defined for the values table, it will be transformed first. After that all value definitions will be transformed to PDF-->
	<xsl:template match="Values">
		<xsl:param name="output"/>
		<xsl:if test="$asn1-fix!=2">
			<xsl:text>VALTAB</xsl:text>
			<xsl:value-of select="$delimiter"/>
			<xsl:value-of select="./ValuesDef/Name"/>
                <xsl:if test="./ValuesDef/Alias">
                    <xsl:value-of select="$delimiter"/>
                    <xsl:text>AS </xsl:text>
                    <xsl:value-of select="./ValuesDef/Alias"/>
                </xsl:if>
			<xsl:apply-templates select="./ValuesDef/Version"/>
			<xsl:value-of select="$newline"/>
			<xsl:for-each select="./ValuesDef/Group">
				<xsl:call-template name="setGroup"/>
			</xsl:for-each>
			<xsl:apply-templates select="." mode="Values-elements">
				<xsl:with-param name="output" select="$output"/>
			</xsl:apply-templates>
		</xsl:if>
	</xsl:template>
	<xsl:template match="Values" mode="Values-elements">
		<xsl:param name="output"/>
		<xsl:for-each select="./ValuesRange">
			<xsl:choose>
				<xsl:when test="$output='PDF'">
					<xsl:text>VAL</xsl:text>
					<xsl:value-of select="$delimiter"/>
					<xsl:call-template name="getValPrefix">
						<xsl:with-param name="valType" select="./@ValueType"/>
					</xsl:call-template>
					<xsl:value-of select="./MinValue"/>
					<xsl:text> - </xsl:text>
					<xsl:call-template name="getValPrefix">
						<xsl:with-param name="valType" select="./@ValueType"/>
					</xsl:call-template>
					<xsl:value-of select="./MaxValue"/>
					<xsl:value-of select="$delimiter"/>
					<xsl:if test="./Alias">
						<xsl:value-of select="./Alias"/>
						<xsl:value-of select="$delimiter"/>					
					</xsl:if>
					<xsl:text>"</xsl:text>
					<xsl:call-template name="stripComment">
						<xsl:with-param name="str" select="./Comment"/>
					</xsl:call-template>
					<xsl:text>"</xsl:text>
				</xsl:when>
				<xsl:when test="$output='MDF'">
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
					<xsl:if test="$asn1-fix!=2">
						<xsl:value-of select="$delimiter"/>
						<xsl:text>"</xsl:text>
						<xsl:call-template name="stripComment">
							<xsl:with-param name="str" select="./Comment"/>
						</xsl:call-template>
						<xsl:text>"</xsl:text>
					</xsl:if>
				</xsl:when>
			</xsl:choose>
			<xsl:value-of select="$newline"/>
		</xsl:for-each>
		<xsl:if test="./ValuesDefault">
			<xsl:choose>
				<xsl:when test="$output='PDF'">
					<xsl:if test="./ValuesDefault/Value">
						<xsl:message terminate="no">
							<xsl:call-template name="cmdLineMsg">
								<xsl:with-param name="msgType" select="warning"/>
								<xsl:with-param name="msg">
									<xsl:text>A default value is defined, but could not be considered for PDF format!</xsl:text>
								</xsl:with-param>
							</xsl:call-template>
						</xsl:message>
					</xsl:if>
					<xsl:text>VAL</xsl:text>
					<xsl:value-of select="$delimiter"/>
					<xsl:text>DEF</xsl:text>
					<xsl:value-of select="$delimiter"/>
				</xsl:when>
				<xsl:when test="$output='MDF'">
					<xsl:choose>
						<xsl:when test="./ValuesDefault/Value">
							<xsl:text>IFNOTPRESENT </xsl:text>
							<xsl:value-of select="./ValuesDefault/Value"/>
						</xsl:when>
						<xsl:otherwise>
							<xsl:text>VAL</xsl:text>
							<xsl:value-of select="$delimiter"/>
							<xsl:text>DEF</xsl:text>
							<xsl:value-of select="$delimiter"/>
						</xsl:otherwise>
					</xsl:choose>
				</xsl:when>
			</xsl:choose>
			<xsl:if test="not($output='MDF' and ./ValuesDefault/Value)">
				<xsl:value-of select="$delimiter"/>
				<xsl:text>"</xsl:text>
				<xsl:call-template name="stripComment">
					<xsl:with-param name="str" select="./ValuesDefault/Comment"/>
				</xsl:call-template>
				<xsl:text>"</xsl:text>
			</xsl:if>
			<xsl:value-of select="$newline"/>
		</xsl:if>
		<xsl:for-each select="./ValuesItem">
			<xsl:text>VAL</xsl:text>
			<xsl:value-of select="$delimiter"/>
			<xsl:choose>
				<xsl:when test="./Value and ./Value!=''">
					<xsl:call-template name="getValPrefix">
						<xsl:with-param name="valType" select="./Value/@ValueType"/>
					</xsl:call-template>
					<xsl:value-of select="./Value"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:text>*</xsl:text>
				</xsl:otherwise>
			</xsl:choose>
			<xsl:value-of select="$delimiter"/>
			<xsl:value-of select="./Alias"/>
			<xsl:apply-templates select="./Version"/>
			<xsl:value-of select="$delimiter"/>
			<xsl:text>"</xsl:text>
			<xsl:call-template name="stripComment">
				<xsl:with-param name="str" select="./Comment"/>
			</xsl:call-template>
			<xsl:text>"</xsl:text>
			<xsl:value-of select="$newline"/>
		</xsl:for-each>
		<xsl:value-of select="$newline"/>
	</xsl:template>
	<!--Version template:This template creates the declaration of a feature flag-->
	<xsl:template match="Version">
		<xsl:value-of select="$delimiter"/>
		<xsl:text>%</xsl:text>
		<xsl:value-of select="."/>
		<xsl:text>%</xsl:text>
	</xsl:template>
	<!--ItemLink template: ItemLink elements will only be processed, if the link points to an external document (DocName of ItemLink is different from DocName of the DocInfoSection). Definition is simple and straight forward.-->
	<xsl:template match="ItemLink" mode="typedef">
		<!--to get the type of the external element (old MDF/PDF format) call template getExternType with elemName = ./Name, docName=./DocName, docType=./DocName/@DocType-->
		<xsl:call-template name="assembleLink"/>
	</xsl:template>
	<xsl:template match="ItemLink">
		<!--If ItemLink references an internal link (same document), then omit the link syntax for now, until ccdGen is extended.-->
		<xsl:if test="./DocName != /*/DocInfoSection/DocName">
			<xsl:text>EXTERN </xsl:text>
			<!--to get the type of the external element (old MDF/PDF format) call template getExternType with elemName = ./Name, docName=./DocName, docType=./DocName/@DocType-->
			<xsl:call-template name="assembleLink"/>
		</xsl:if>
	</xsl:template>
	<!--Substitute template:This template creates the definition for a substitution-->
	<xsl:template match="Substitute">
		<xsl:text>TYPEDEF</xsl:text>
		<xsl:value-of select="$delimiter"/>
		<xsl:call-template name="getExternType">
			<xsl:with-param name="elemName" select="./ItemLink/Name"/>
			<xsl:with-param name="docType" select="./ItemLink/DocName/@DocType"/>
			<xsl:with-param name="docName" select="./ItemLink/DocName"/>
			<xsl:with-param name="transformType" select="'varOnly'"/>
		</xsl:call-template>
		<xsl:value-of select="$delimiter"/>
		<xsl:choose>
			<!--If ItemLink references an internal link (same document), then omit the link syntax for now, until ccdGen is extended.-->
			<xsl:when test="./ItemLink/DocName != /*/DocInfoSection/DocName">
				<xsl:for-each select="./ItemLink">
					<xsl:call-template name="assembleLink"/>
				</xsl:for-each>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="./ItemLink/Name"/>
			</xsl:otherwise>
		</xsl:choose>
		<xsl:value-of select="$delimiter"/>
		<xsl:value-of select="./Alias"/>
		<xsl:apply-templates select="./Version"/>
		<xsl:value-of select="$delimiter"/>
		<xsl:text>"</xsl:text>
		<xsl:call-template name="stripComment">
			<xsl:with-param name="str" select="./Comment"/>
		</xsl:call-template>
		<xsl:text>"</xsl:text>
		<xsl:value-of select="$newline"/>
	</xsl:template>
	<!--named template to add the group name-->
	<xsl:template name="setGroup">
		<xsl:value-of select="$delimiter"/>
		<xsl:text>GROUP </xsl:text>
		<xsl:value-of select="."/>
		<xsl:value-of select="$newline"/>
	</xsl:template>
	<!--named template to assemble a link description in PDF-->
	<xsl:template name="assembleLink">
		<xsl:choose>
			<xsl:when test='./DocName[@DocType="SAP"]'>
				<xsl:text>@p_</xsl:text>
			</xsl:when>
			<xsl:when test='./DocName[@DocType="AIM"]'>
				<xsl:text>@m_</xsl:text>
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
		<xsl:value-of select="./DocName"/>
		<xsl:text> - </xsl:text>
		<xsl:value-of select="./Name"/>
		<xsl:text>@</xsl:text>
		<xsl:value-of select="$delimiter"/>
	</xsl:template>
	<!--named template to assemble the definition of a structured element-->
	<xsl:template name="StructElemDef">
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
		<xsl:apply-templates select="./Version"/>
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
	</xsl:template>
	<!--named template extract relevant text for PDF/MDF comments-->
	<xsl:template name="stripComment">
		<xsl:param name="str"/>
		<xsl:variable name="dblQts">
			<xsl:text>"</xsl:text>
		</xsl:variable>
		<xsl:variable name="sglQts">
			<xsl:text>'</xsl:text>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="contains($str,$dblQts)">
				<!--exchange double quotes with single quotes-->
				<xsl:value-of select="translate($str,  $dblQts, $sglQts )"/>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="$str"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	<!--named template add file status pragmas-->
	<xsl:template name="fileStatPragmas">
		<xsl:variable name="fileName">
			<xsl:choose>
				<xsl:when test="*/DocInfoSection/DocName/@DocType = 'SAP'">
					<xsl:value-of select="concat($absSapPath,*/DocInfoSection/DocName,'.sap')"/>
				</xsl:when>
				<xsl:otherwise>
					<xsl:value-of select="concat($absAimPath,*/DocInfoSection/DocName,'.aim')"/>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:variable>
		<xsl:value-of select="concat('PRAGMA',$delimiter,'SRC_FILE_TIME')"/>
		<xsl:value-of select="$delimiter"/>
		<xsl:text>"</xsl:text>
		<xsl:value-of select="ext-pack:FileDate.getFileDate($fileName )"/>
		<xsl:text>"</xsl:text>
		<xsl:value-of select="$newline"/>
		<xsl:for-each select="*/DocInfoSection/DocHistory">
			<xsl:if test="position()=last()">
				<xsl:value-of select="concat('PRAGMA',$delimiter,'LAST_MODIFIED')"/>
				<xsl:value-of select="$delimiter"/>
				<xsl:text>"</xsl:text>
				<xsl:apply-templates select="./Date"/>
				<xsl:text>"</xsl:text>
				<xsl:value-of select="$newline"/>
				<xsl:value-of select="concat('PRAGMA',$delimiter,'ID_AND_VERSION')"/>
				<xsl:value-of select="$delimiter"/>
				<xsl:text>"</xsl:text>
				<xsl:apply-templates select="." mode="DocID"/>
				<xsl:text>"</xsl:text>
				<xsl:value-of select="$newline"/>
			</xsl:if>
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>

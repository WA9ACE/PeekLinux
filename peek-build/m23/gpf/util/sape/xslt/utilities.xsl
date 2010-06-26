<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:import href="format.xsl"/>
	<!--named template to format a command line message-->
	<xsl:template name="cmdLineMsg">
		<xsl:param name="msgType"/>
		<xsl:param name="msg"/>
		<xsl:choose>
			<xsl:when test='$msgType="error"'>
				<xsl:text>### FATAL ERROR: </xsl:text>
			</xsl:when>
			<xsl:when test='$msgType="warning"'>
				<xsl:text>### WARNING: </xsl:text>
			</xsl:when>
			<xsl:when test='$msgType="info"'>
				<xsl:text>### INFO: </xsl:text>
			</xsl:when>
		</xsl:choose>
		<xsl:value-of select="$msg"/>
		<xsl:value-of select="$newline"/>
	</xsl:template>
	<!--named template to convert string characters into lower case-->
	<xsl:template name="toLower">
		<xsl:param name="str"/>
		<xsl:value-of select="translate($str, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz' )"/>
	</xsl:template>
	<!--named template to convert string characters into upper case-->
	<xsl:template name="toUpper">
		<xsl:param name="str"/>
		<xsl:value-of select="translate($str,  'abcdefghijklmnopqrstuvwxyz', 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' )"/>
	</xsl:template>
	<!--named template to add the number type prefix for a Value element-->
	<xsl:template name="getValPrefix">
		<xsl:param name="valType"/>
		<xsl:choose>
			<xsl:when test='$valType="DEC"'/>
			<xsl:when test='$valType="ALPHA"'/>
			<xsl:when test='$valType="HEX"'>
				<xsl:text>0x</xsl:text>
			</xsl:when>
			<xsl:when test='$valType="BIN"'>
				<xsl:text>0b</xsl:text>
			</xsl:when>
			<xsl:when test='$valType="OCT"'>
				<xsl:text>0</xsl:text>
			</xsl:when>
			<xsl:otherwise>
				<xsl:message terminate="yes">
					<xsl:call-template name="cmdLineMsg">
						<xsl:with-param name="msgType" select="error"/>
						<xsl:with-param name="msg">
							<xsl:text>Value.@ValueType is neither DEC nor ALPHA nor HEX nor BIN nor OCT</xsl:text>
						</xsl:with-param>
					</xsl:call-template>
				</xsl:message>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
</xsl:stylesheet>

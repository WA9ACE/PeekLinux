<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<!-- template to format a complete document ID expression-->
	<xsl:template match="DocHistory" mode="DocID">
		<xsl:value-of select="../DocNum/@Project"/>
		<xsl:text>.</xsl:text>
		<xsl:value-of select="../DocNum/@Number"/>
		<xsl:text>.</xsl:text>
		<xsl:value-of select="./DocVersion/@Year"/>
		<xsl:text>.</xsl:text>
		<xsl:value-of select="./DocVersion/@Number"/>
	</xsl:template>
</xsl:stylesheet>

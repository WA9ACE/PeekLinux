<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" extension-element-prefixes="ext-pack" xmlns:ext-pack="xalan://com.ti.xslt.extension">
	<!-- template to format a date expression-->
	<xsl:template match="Date">
            <xsl:value-of select="ext-pack:DateString.format(./@Year, ./@Month, ./@Day)"/>
	</xsl:template>
</xsl:stylesheet>

<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:ext-pack="xalan://com.ti.xslt.extension" extension-element-prefixes="ext-pack">
    <xsl:output method="text"/>
    <!--PrimID template: This template will be used to assemble the hexadecimal value of the primitive identifier out of Number and Direction of the PrimID element and SAPid of the PrimitivesSection element. The values will be passed to an external java class, which performs the calculation for 16 bit and 32 bit primitive identifier types.-->
    <xsl:template match="PrimID">
        <xsl:param name="memHandle" select="false()"/>
        <xsl:variable name="primDir">
            <xsl:choose>
                <xsl:when test='./@Direction="UPLINK"'>
                    <xsl:text>0</xsl:text>
                </xsl:when>
                <xsl:when test='./@Direction="DOWNLINK"'>
                    <xsl:text>1</xsl:text>
                </xsl:when>
                <xsl:otherwise>
                    <xsl:message terminate="yes">
                        <xsl:call-template name="cmdLineMsg">
                            <xsl:with-param name="msgType" select="error"/>
                            <xsl:with-param name="msg">
                                <xsl:text>PrimID@Direction is neither UPLINK nor DOWNLINK</xsl:text>
                            </xsl:with-param>
                        </xsl:call-template>
                    </xsl:message>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        <xsl:choose>
            <xsl:when test='/SAP/PrimitivesSection[@PrimIDType="BIT32"]'>
                <!-- Use customized java class of extension packet to get hexadecimal primitive identifier. -->
                <xsl:value-of select="ext-pack:PrimIdentifier.get32BitPrimId(./@Number, $primDir, /SAP/PrimitivesSection/@SAPid, $memHandle )"/>
            </xsl:when>
            <xsl:when test='/SAP/PrimitivesSection[@PrimIDType="BIT32_VIRTUAL"]'>
                <!-- Use customized java class of extension packet to get hexadecimal primitive identifier for internal purposes. -->
                <xsl:value-of select="ext-pack:PrimIdentifier.get32BitPrimId(./@Number, $primDir, /SAP/PrimitivesSection/@SAPid, $memHandle, true() )"/>
            </xsl:when>
            <xsl:when test='/SAP/PrimitivesSection[@PrimIDType="BIT16"]'>
                <!-- Use customized java class of extension packet to get hexadecimal primitive identifier. -->
                <xsl:value-of select="ext-pack:PrimIdentifier.get16BitPrimId(./@Number, $primDir, /SAP/PrimitivesSection/@SAPid )"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:message terminate="yes">
                    <xsl:call-template name="cmdLineMsg">
                        <xsl:with-param name="msgType" select="error"/>
                        <xsl:with-param name="msg">
                            <xsl:text>PrimitivesSection.@PrimIDType is neither BIT32 nor BIT16</xsl:text>
                        </xsl:with-param>
                    </xsl:call-template>
                </xsl:message>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>
</xsl:stylesheet>

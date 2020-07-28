<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="2.0">
    <xsl:template match="/">
        <html>
            <body style="font-family:sans-serif;font-size:12pt;color:blue">
                <h2 align="center">xdump command-line options</h2>
                <xsl:for-each select="xdump/section">
                    <xsl:if test="normalize-space(@title) = 'Data field options'">
                        <div style="page-break-before:always"/>
                    </xsl:if>
                    <h4 align="left">
                        <xsl:value-of select="@title"/>
                    </h4>
                    <h5 align="left">
                        <table border="1">
                            <tr>
                                <th> Short option </th>
                                <th> Long option </th>
                                <th> Argument? </th>
                                <th> Default </th>
                                <th>Help text</th>
                            </tr>
                            <xsl:for-each select="option">
                                <tr>
                                    <xsl:attribute name="style">
                                        <xsl:choose>
                                            <xsl:when test="normalize-space(short_name) != ''"></xsl:when>
                                            <xsl:otherwise>color:red</xsl:otherwise>
                                        </xsl:choose>
                                    </xsl:attribute>
                                    <td align="center">
                                        <xsl:choose>
                                            <xsl:when test="normalize-space(short_name) = ''">(n/a)</xsl:when>
                                            <xsl:otherwise>-<xsl:value-of select="short_name"/></xsl:otherwise>
                                        </xsl:choose>
				    </td>
                                    <td> --<xsl:value-of select="long_name"/> </td>
                                    <td align="center">
                                        <xsl:choose>
                                            <xsl:when test="argument">
                                                <xsl:choose>
                                                    <xsl:when test="argument = 'required'">Required</xsl:when>
                                                    <xsl:when test="argument = 'optional'">Optional</xsl:when>
                                                    <xsl:otherwise>No</xsl:otherwise>
                                                </xsl:choose>
                                            </xsl:when>
                                            <xsl:otherwise>No</xsl:otherwise>
                                        </xsl:choose>
                                    </td>
                                    <td align="center">
                                        <xsl:if test="default">
                                            <xsl:text>-</xsl:text>
                                            <xsl:value-of select="short_name"/>
                                            <xsl:if test="normalize-space(default) != ''">
                                                <xsl:text> </xsl:text>
                                                <xsl:value-of select="default"/>
                                            </xsl:if>
                                        </xsl:if>
                                    </td>
                                    <td>
                                        <xsl:for-each select="help">
                                            <xsl:text> </xsl:text>
                                            <xsl:value-of select="."/>
                                            <xsl:text> </xsl:text>
                                            <br/>
                                        </xsl:for-each>
                                    </td>
                                </tr>
                            </xsl:for-each>
                        </table>
                    </h5>
                </xsl:for-each>
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>

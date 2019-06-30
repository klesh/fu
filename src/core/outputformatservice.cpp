#include "error.h"
#include "outputformatservice.h"

OutputFormat convertResultToOutputFormat(QSqlQuery &result) {
    OutputFormat outputFormat;
    auto rec = result.record();
    outputFormat.id = result.value(rec.indexOf("id")).toUInt();
    outputFormat.name = result.value(rec.indexOf("name")).toString();
    outputFormat.templateTEXT = result.value(rec.indexOf("template")).toString();
    return outputFormat;
}

OutputFormatService::OutputFormatService(SqlStore &store)
    : _store(store)
{
}

QList<OutputFormat> OutputFormatService::getAll()
{
    QList<OutputFormat> outputFormats;
    auto result = _store.exec("SELECT * FROM outputFormats ORDER BY id DESC");
    while (result.next()) {
        outputFormats.append(convertResultToOutputFormat(result));
    }
    return outputFormats;
}

OutputFormat OutputFormatService::findById(uint id)
{
    auto query = _store.prepare("SELECT * FROM outputFormats WHERE id=:id");
    query.bindValue(":id", id);
    auto result = _store.exec();
    if (!result.next()) {
        throw_error("OutputFormat not found");
    }
    return convertResultToOutputFormat(result);
}

void OutputFormatService::append(OutputFormat &outputFormat)
{
    auto query = _store.prepare("INSERT INTO outputFormats (name, template, enabled) VALUES (:name, :template, :enabled)");
    query.bindValue(":name", outputFormat.name);
    query.bindValue(":template", outputFormat.templateTEXT);

    auto result = _store.exec();
    outputFormat.id = result.lastInsertId().toUInt();
}

void OutputFormatService::update(OutputFormat &outputFormat)
{
    auto query = _store.prepare("UPDATE outputFormats SET name=:name, template=:template, enabled=:enabled WHERE id=:id");
    query.bindValue(":name", outputFormat.name);
    query.bindValue(":template", outputFormat.templateTEXT);
    query.bindValue(":id", outputFormat.id);
    _store.exec();
}

void OutputFormatService::save(OutputFormat &outputFormat)
{
    if (outputFormat.id) {
        update(outputFormat);
    } else {
        append(outputFormat);
    }

}

void OutputFormatService::remove(uint id)
{
    auto query = _store.prepare("DELETE FROM outputFormats where id=:id");
    query.bindValue(":id", id);
    _store.exec();
}

QString OutputFormatService::format(const OutputFormat &outputFormat, const Clip &clip, const Upload &upload)
{
    if (outputFormat.templateTEXT.isEmpty())
        return QString();

    qDebug() << outputFormat.templateTEXT << upload.rawOutput << clip.description;
    return outputFormat.templateTEXT.arg(upload.rawOutput).arg(clip.description);
}

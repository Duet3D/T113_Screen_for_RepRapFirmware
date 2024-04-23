/*
 * Files.h
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_OBJECTMODEL_FILES_HPP_
#define JNI_OBJECTMODEL_FILES_HPP_

#include "control/ZKListView.h"
#include <string>
#include <vector>

namespace OM::FileSystem
{
	enum class FileSystemItemType
	{
		folder,
		file,
	};

	class FileSystemItem
	{
	public:
	  FileSystemItem(const FileSystemItemType type) : m_type(type), m_size(0) {}
	  FileSystemItem(const FileSystemItemType type, const std::string& name) : m_name(name), m_type(type), m_size(0) {}

	  const std::string& GetName() const { return m_name; }
	  void SetName(const std::string name);
	  std::string GetPath() const;
	  const std::string& GetDate() const { return m_date; }
	  void SetDate(const std::string& date) { m_date = date; }
	  size_t GetSize() const { return m_size; }
	  std::string GetReadableSize() const;
	  void SetSize(const size_t size) { m_size = size; }
	  FileSystemItemType GetType() const { return m_type; }

	  ~FileSystemItem();
	private:
	  std::string m_name;
	  FileSystemItemType m_type;
	  size_t m_size;
	  std::string m_date;
	};

	class File : public FileSystemItem
	{
	public:
		File() : FileSystemItem(FileSystemItemType::file) {}
		File(const std::string& name) : FileSystemItem(FileSystemItemType::file, name) {}
	};


	class Folder : public FileSystemItem
	{
	public:
		Folder() : FileSystemItem(FileSystemItemType::folder) {}
		Folder(const std::string& name) : FileSystemItem(FileSystemItemType::folder, name) {}
	};

	void Init(ZKTextView* folderId, ZKListView* listView);
	ZKListView* GetListView();
	File* AddFileAt(const size_t index);
	Folder* AddFolderAt(const size_t index);
	const size_t GetItemCount();
	const std::vector<FileSystemItem*>& GetItems();
	FileSystemItem* GetItem(const size_t index);
	File* GetFile(const std::string& name);
	Folder* GetSubFolder(const std::string& name);
	void SetCurrentDir(const std::string& path);
	void SortFileSystem();
	std::string GetParentDirPath();
	std::string GetCurrentDirName();
	std::string& GetCurrentDirPath();
	bool IsInSubFolder();
	void RequestFiles(const std::string& path);
	void RequestUsbFiles(const std::string& path);
	bool IsMacroFolder();
	bool IsUsbFolder();
	void RunFile(const File* file);
	void RunMacro(const std::string& path);
	void UploadFile(const File* file);
	void StartPrint(const std::string& path);
	void ResumePrint();
	void PausePrint();
	void StopPrint();
	void ClearFileSystem();

	std::string GetFileExtension(const std::string& fileName);
} /* namespace OM */

#endif /* JNI_OBJECTMODEL_FILES_HPP_ */

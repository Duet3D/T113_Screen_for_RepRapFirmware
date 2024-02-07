/*
 * Files.h
 *
 *  Created on: 3 Jan 2024
 *      Author: Andy Everitt
 */

#ifndef JNI_LOGIC_OBJECTMODEL_FILES_HPP_
#define JNI_LOGIC_OBJECTMODEL_FILES_HPP_

#include "Debug.h"
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
		FileSystemItem(const FileSystemItemType type) : type_(type), size_(0) {
			dbg("Files: Creating item of type %d", (int)type_);
		}
		FileSystemItem(const FileSystemItemType type, const std::string& name) : name_(name), type_(type), size_(0) {}

		const std::string& GetName() const { return name_; }
		void SetName(const std::string name);
		std::string GetPath() const;
		const std::string& GetDate() const { return date_; }
		void SetDate(const std::string& date) { date_ = date; }
		size_t GetSize() const { return size_; }
		void SetSize(const size_t size) { size_ = size; }
		FileSystemItemType GetType() const { return type_; }

		~FileSystemItem();
	private:
		std::string name_;
		FileSystemItemType type_;
		size_t size_;
		std::string date_;
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
	File* AddFileAt(const size_t index);
	Folder* AddFolderAt(const size_t index);
	const size_t GetItemCount();
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
} /* namespace OM */



#endif /* JNI_LOGIC_OBJECTMODEL_FILES_HPP_ */
